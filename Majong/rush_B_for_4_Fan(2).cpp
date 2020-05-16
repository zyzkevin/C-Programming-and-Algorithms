//清龙+一色三步高+五门齐+七对
#ifndef _BOTZONE_ONLINE
#define _DEBUG_ // 开启Debug模式
#endif
// #define _MATCH_MODE_ //新赛制
#define _LONG_TIME_ //使用长时运行
#define _QL_YS_     //作为一色三步高，清龙的标记
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <unordered_map>
#include <map>
#ifdef _BOTZONE_ONLINE
#include "MahjongGB/fan_calculator.cpp"
#include "MahjongGB/shanten.cpp"
#else
#include "mahjong-algorithm/tile.h"
#include "mahjong-algorithm/fan_calculator.cpp"
#include "mahjong-algorithm/shanten.cpp"
#endif

#include "jsoncpp/json.h"

using namespace std;
using namespace mahjong;

vector<string> responsesVector, requestsVector; //用于储存输入输出,

vector<tile_t> hand;

vector<pack_t> mingPai[4]; //明牌，记录4个人的

int quan; //风圈
// int flowerCount[4];//补花数，对决策无影响,也不计入算番
int playerID; //门风

unordered_map<tile_t, int> handCount; //对手牌中的牌计数，用时更新数据

unordered_map<tile_t, int> unknown; //未知的牌(已知:排池中的牌+明牌+自家暗牌)剩余的数量,用以估价,花牌不考虑

unordered_map<tile_t, int> paiChi; //牌池中的牌，用于分析决张番形

int tileWall[4] = {36, 36, 36, 36}; //分析牌墙剩余牌数，计算海底捞月等牌型

tile_t lastPlay; //最后一张打出的手牌编号，用以判定碰杠吃

int lastPlayer; //最后一个出牌人

int xiaJia; //最后一个出牌人的下家

tile_t lastDrawCard; //自己抽到的最后一张牌

static unordered_map<string, tile_t> str2tile;

enum
{
    QIDUI,
    WUMENQI,
    QINGLONG,
    YISESANBUGAO,
    SANSESANBUGAO,
    SANSESANTONGSHUN,
    HUALONG,
}; //七对，五门齐，清龙，一色三步高,三色三步高，三色三同顺，花龙。

int selfTargetFan = QIDUI;

//特定番形分析所需参数
//清龙一色三步高
unordered_map<tile_t, double> QLYSvalue;
int colorCount[4] = {0, 0, 0, 0};
suit_t mainColor;
//七对
unordered_map<tile_t, double> QDvalue; //估价数组
//五门齐
bool WUMEN_Count[6] = {0, 0, 0, 0, 0, 0}; //统计五门是否已齐;
unordered_map<tile_t, double> WMvalue;
//三色三同顺，三色三步高
unordered_map<tile_t, double> SSvalue; //三色类估价
rank_t SS_CentralRank;                 //三同顺中处于中心的牌

inline void GetHandCount()
{
    for (tile_t a : all_tiles)
    {
        handCount[a] = 0;
    }
    for (tile_t a : hand)
    {
        ++handCount[a];
    }
}

inline bool TileWallOut() //牌墙耗尽
{
#ifdef _MATCH_MODE_
    return tileWall[xiaJia] == 0;
#endif
    int sum = 0;
    for (int i = 0; i < 4; ++i)
    {
        sum += tileWall[i];
    }
    return sum == 0;
}

inline int GetRelativeID(int player_) //将绝对的排序转换为上下对家
{
    return (playerID + 4 - player_) % 4;
}

inline int WhoDraw(int turn_) //判定该回合是否是摸牌回合,如是则返回相应玩家，反之返回-1
{
    istringstream sin(requestsVector[turn_]);
    int type, who;
    string act;
    sin >> type;
    if (type <= 2)
    {
        return playerID;
    }
    sin >> who >> act;
    if (act == "DRAW")
    {
        return who;
    }
    return -1;
}

inline string GetStr(const tile_t x_) //给定牌编号，获得相应字符串，花牌只在判断牌墙剩余张数时有用
{
    int rank = tile_get_rank(x_);
    switch (tile_get_suit(x_))
    {
    case TILE_SUIT_CHARACTERS:
        return "W" + to_string(rank);
        break;
    case TILE_SUIT_DOTS:
        return "B" + to_string(rank);
        break;
    case TILE_SUIT_BAMBOO:
        return "T" + to_string(rank);
        break;
    case TILE_SUIT_HONORS:
        if (rank <= 4)
        {
            return "F" + to_string(rank);
        }
        else
        {
            return "J" + to_string(rank - 4);
        }
    default:
        return string();
        break;
    }
}

inline tile_t GetTile_t(const string &s_) //给定字符串，获得相应编号
{
    return str2tile[s_];
}

inline void PlayCard(int who_, tile_t s_) //实现将牌放入牌池，消除未知,更新最后打出的牌
{
    lastPlay = s_;
    if (who_ != playerID)
    {
        --unknown[lastPlay];
    }
    ++paiChi[s_];
    lastPlayer = who_;
}

inline void DeleteHand(tile_t s_) //将自己打出的牌从手牌中删除
{
#ifdef _DEBUG_
    cout << "delete" << GetStr(s_) << endl;
#endif //  _DEBUG_
    for (auto iter = hand.begin(); iter != hand.end(); ++iter)
    {
        if (*iter == s_)
        {
            hand.erase(iter);
            return;
        }
    }
    return;
}

bool Hu(tile_t winTile_, bool isZIMO_, bool isGANG_)
{
    static calculate_param_t calculate_param;
    static fan_table_t fan_table;
    memset(&calculate_param, 0, sizeof(mahjong::calculate_param_t));
    memset(&fan_table, 0, sizeof(mahjong::fan_table_t));
    calculate_param.hand_tiles.tile_count = hand.size();
    for (unsigned int i = 0; i < hand.size(); i++)
    {
        calculate_param.hand_tiles.standing_tiles[i] = hand[i];
    }
    calculate_param.hand_tiles.pack_count = mingPai[playerID].size();
    for (unsigned int i = 0; i < mingPai[playerID].size(); i++)
    {
        calculate_param.hand_tiles.fixed_packs[i] = mingPai[playerID][i];
    }
    calculate_param.win_tile = winTile_;
    calculate_param.flower_count = 0;
    if (isZIMO_)
    {
        calculate_param.win_flag |= WIN_FLAG_SELF_DRAWN;
    }
    if (TileWallOut())
    {
        calculate_param.win_flag |= WIN_FLAG_WALL_LAST;
    }
    if (paiChi[winTile_] >= (4 - isZIMO_))
    {
        calculate_param.win_flag |= WIN_FLAG_4TH_TILE;
    }
    if (isGANG_)
    {
        calculate_param.win_flag |= WIN_FLAG_ABOUT_KONG;
    }
    calculate_param.prevalent_wind = (mahjong::wind_t)quan;
    calculate_param.seat_wind = (mahjong::wind_t)playerID;
    int re = calculate_fan(&calculate_param, &fan_table);
    return re >= 8;
}

bool CanPENG(int who_, tile_t card_) //判定能不能碰
{
    if (who_ == playerID)
    {
        return 0;
    }
    if (TileWallOut() && xiaJia != playerID)
    {
        return 0;
    }
    int count = 0;
    for (auto iter = hand.begin(); iter != hand.end(); ++iter)
    {
        count += (*iter == card_);
    }
    bool ret = (count == 2);
    //分析吃对番形是否有利
    suit_t tmp_suit = tile_get_suit(card_);
    switch (selfTargetFan)
    {
    case QIDUI:
        return ret = 0;
        break;
    case WUMENQI:

        if (WUMEN_Count[tmp_suit])
        {
            ret = 0;
        }
        break;
    case QINGLONG:
        ret = 0;
        break;
    case YISESANBUGAO:
        ret = 0;
        break;
    case SANSESANBUGAO:
        ret = 0;
        break;
    case SANSESANTONGSHUN:
        ret = 0;
        break;
    default:
        ret = 0;
        break;
    }
    return ret;
}

void PENG(int who_, tile_t card_) //碰后将牌组从手牌移到明牌
{

    DeleteHand(card_);
    DeleteHand(card_);
    mingPai[playerID].push_back(make_pack(GetRelativeID(who_), PACK_TYPE_PUNG, card_));
}

bool CanMINGGANG(int who_, tile_t card_) //判定能不能明杠,算番库中通过供牌人是否是自己来判定明暗杠
{
    return 0;
    if (TileWallOut() && xiaJia != playerID)
    {
        return 0;
    }
    if (who_ == playerID)
    {
        return 0;
    }
    int count = 0;
    for (auto iter = hand.begin(); iter != hand.end(); ++iter)
    {
        count += (*iter == card_);
    }
    return count >= 3;
}

void MINGGANG(int who_, tile_t card_) //明杠
{
    string tmpCard = GetStr(card_);
    for (int i = 0; i < 3; ++i)
    {
        DeleteHand(card_);
    }
    mingPai[playerID].push_back(make_pack(GetRelativeID(who_), PACK_TYPE_KONG, card_)); //如供牌人是自己则为暗杠;
}

bool CanANGANG(tile_t card_) //判定能不能暗杠,算番库中通过供牌人是否是自己来判定明暗杠
{
    if (selfTargetFan != WUMENQI)
    {
        return 0;
    }
    if (TileWallOut() && xiaJia != playerID)
    {
        return 0;
    }
    int count = 0;
    for (auto iter = hand.begin(); iter != hand.end(); ++iter)
    {
        count += (*iter == card_);
    }
    return count >= 4;
}

void ANGANG(tile_t card_) //暗杠
{
    string tmpCard = GetStr(card_);
    for (int i = 0; i < 4; ++i)
    {
        DeleteHand(card_);
    }
    mingPai[playerID].push_back(make_pack(0, PACK_TYPE_KONG, card_)); //如供牌人是自己则为暗杠;
}

bool CanANBUGANG(tile_t card_) //暗补杠，但从对局来看，只能补杠自己摸的牌
{
    //对目前的几个番形无需分析
    return 0;
    if (TileWallOut() && xiaJia != playerID)
    {
        return 0;
    }
    for (auto a : mingPai[playerID])
    {
        if (pack_get_type(a) == PACK_TYPE_PUNG)
        {
            if (pack_get_tile(a) == card_)
            {
                return 1;
            }
        }
    }
    return 0;
};

void ANBUGANG(int card_) //
{
    for (auto iter = mingPai[playerID].begin();
         iter != mingPai[playerID].end(); ++iter)
    {
        if (pack_get_type(*iter) == PACK_TYPE_KONG)
        {
            if (pack_get_tile(*iter) == card_)
            {
                *iter = promote_pung_to_kong(*iter);
                return;
            }
        }
    }
}

bool CanCHI(int who_, tile_t card_, tile_t &retMidCard)
{
    int count[5] = {0, 0, 1, 0, 0}; //分别计数比打出牌-2,-1,1,2的牌
    retMidCard = 0;
    bool ret = 0;
    if (GetRelativeID(who_) != 1)
    {
        return 0;
    }
    if (tile_get_suit(card_) > 3)
    {
        return 0;
    }
    for (auto iter = hand.begin(); iter != hand.end(); ++iter)
    {
        for (int i = 0; i < 5; ++i)
        {
            count[i] += (*iter == card_ - 2 + i);
        }
    }
    //由于不同种牌间的编号有空隙，所以可行
    for (int i = 0; i < 3; ++i)
    {
        if (count[i] && count[i + 1] && count[i + 2])
        {
            retMidCard = card_ - 1 + i;
            ret = 1;
        }
    }
    //分析吃对番形是否有利
    suit_t tmp_suit;
    switch (selfTargetFan)
    {
    case QIDUI:
        return 0;
        break;
    case WUMENQI:
        tmp_suit = tile_get_suit(retMidCard);
        if (tmp_suit <= 3 && WUMEN_Count[tmp_suit])
        {
            ret = 0;
        }
        break;
    case QINGLONG:
        GetHandCount();
        if (tile_get_suit(card_) == mainColor)
        {
            if (!handCount[card_])
            {
                if (retMidCard != make_tile(mainColor, 2))
                {
                    if (retMidCard != make_tile(mainColor, 5))
                    {
                        if (retMidCard != make_tile(mainColor, 8))
                        {
                            ret = 0;
                        }
                    }
                }
            }
            else
            {
                ret = 0;
            }
        }
        else
        {
            ret = 0;
        }
        break;
    case YISESANBUGAO:
        GetHandCount();
        if (tile_get_suit(card_) == mainColor)
        {
            if (handCount[card_] >= 2)
            {
                ret = 0;
            }
        }
        else
        {
            ret = 0;
        }
        break;
    case SANSESANTONGSHUN:
        GetHandCount();
        if (tile_get_rank(retMidCard) == SS_CentralRank)
        {
            if (handCount[card_])
            {
                ret = 0;
            }
            else
            {
                for (pack_t tmp_pack : mingPai[playerID])
                {
                    if (pack_get_tile(tmp_pack) == retMidCard)
                    {
                        ret = 0;
                    }
                }
            }
        }
        else
        {
            ret = 0;
        }
        break;
    case SANSESANBUGAO:
        GetHandCount();
        if (tile_get_rank(retMidCard) == SS_CentralRank)
        {
            if (handCount[card_])
            {
                ret = 0;
            }
        }
        else
        {
            ret = 0;
        }
        break;
    }
    return ret;
}

void CHI(tile_t card_, tile_t midcard_)
{
    bool waitForDelete[3] = {1, 1, 1};
    waitForDelete[card_ - midcard_ + 1] = 0;
    for (int i = 0; i < 3; ++i)
    {
        if (waitForDelete[i])
        {
            DeleteHand(midcard_ - 1 + i);
        }
    }
    mingPai[playerID].push_back(make_pack(1, PACK_TYPE_CHOW, midcard_));
}

void SortHand_QLYS()
{

    GetHandCount();

    for (tile_t a : all_tiles)
    {
        if (tile_get_suit(a) <= 3)
        {
            ++colorCount[tile_get_suit(a)];
        }
    }

    for (suit_t i = 1; i <= 3; ++i)
    {
        if (colorCount[i] > colorCount[mainColor])
        {
            mainColor = i;
        }
    }
    for (rank_t r = 1; r <= 9; ++r)
    {
        if (!unknown[make_tile(mainColor, r)])
        {
            selfTargetFan = YISESANBUGAO;
            break;
        }
    }
    if (selfTargetFan == QINGLONG)
    {
        for (tile_t a : all_tiles)
        {
            if (tile_get_suit(a) != mainColor)
            {
                QLYSvalue[a] = -1;
            }
            else if (handCount[a] > 2)
            {
                QLYSvalue[a] = 0;
            }
            else if (handCount[a] == 2)
            {
                QLYSvalue[a] = 0.9;
            }
            else if (handCount[a] == 1)
            {
                QLYSvalue[a] = 1;
            }
        }
    }
    else if (selfTargetFan == YISESANBUGAO)
    {
        for (tile_t a : all_tiles)
        {
            if (tile_get_suit(a) != mainColor)
            {
                QLYSvalue[a] = -1;
            }
            else if (handCount[a] > 2)
            {
                QLYSvalue[a] = 0;
            }
            else if (handCount[a] == 2)
            {
                QLYSvalue[a] = 1.1;
            }
            else if (handCount[a] == 1)
            {
                QLYSvalue[a] = 1;
            }
            QLYSvalue[a] = handCount[a] * 0.8 + unknown[a] * 0.3 +
                           (handCount[a - 1] + handCount[a + 1]) * 0.9 + (unknown[a - 1] + unknown[a + 1]) * 0.4;

            if (tile_get_rank(a) > 1 && (handCount[a - 1] + unknown[a - 1]) > 0)
            {
                QLYSvalue[a] += 0.85 * handCount[a - 2] + 0.35 * handCount[a + 2];
            }
            if (tile_get_rank(a) < 9 && (handCount[a + 1] + unknown[a + 1]) > 0)
            {
                QLYSvalue[a] = handCount[a + 2] * 0.85 + unknown[a + 2] * 0.35;
            }
        }
    }
    sort(hand.begin(), hand.end(),
         [](tile_t a, tile_t b) {
             return QLYSvalue[a] > QLYSvalue[b];
         });
}

void SortHand_QIDUI()
{
    //超过两个则优先打出，再按剩余的牌数估计机会

    int unknownSum = 0;
    for (tile_t a : all_tiles)
    {
        unknownSum += unknown[a];
        QDvalue[a] = unknown[a];
    }
    GetHandCount();
    for (tile_t a : all_tiles)
    {
        if (unknown[a] + handCount[a] < 2)
        {
            QDvalue[a] = -1;
        }
        else if (handCount[a] == 2)
        {
            QDvalue[a] = 100;
        }
        else if (handCount[a] > 2)
        {
            QDvalue[a] = 0;
        }
        else if (handCount[a] == 1)
        {
            QDvalue[a] = ((double)unknown[a]) / unknownSum;
        }
        else if (handCount[a] == 0)
        {
            QDvalue[a] = (unknown[a]) * (unknown[a] - 1) / (double)((unknown[a]) * (unknown[a] - 1)) * 0.9; //延长和牌时间有风险
        }
    }
    sort(hand.begin(), hand.end(),
         [](tile_t a, tile_t b) {
             return QDvalue[a] > QDvalue[b];
         });
}

void SortHand_WUMENQI()
{
    GetHandCount();

    static bool init = 1;
    if (init)
    {
        for (tile_t a : all_tiles)
        {
            WMvalue[a] = 1 + (tile_get_suit(a) == TILE_SUIT_HONORS);
        }
        init = 0;
    } //如果某一种牌已经凑成了面子，则这一种的其他牌为估价为0
    for (pack_t p : mingPai[playerID])
    {
        suit_t tmp_suit = (tile_get_suit(pack_get_tile(p)));
        if (tmp_suit <= 3)
        {
            WUMEN_Count[tmp_suit] = 1;
            for (tile_t a : all_tiles)
            {
                if (tile_get_suit(a) == tmp_suit)
                {
                    WMvalue[a] = -1;
                }
            }
        }
        else
        {
            if (tile_get_rank(pack_get_tile(p)) <= 4)
            {
                WUMEN_Count[4] = 1;
                for (rank_t tmp_rank = 1; tmp_rank <= 4; ++tmp_rank)
                {
                    WMvalue[make_tile(tmp_suit, tmp_rank)] = -1;
                }
            }
            else
            {
                WUMEN_Count[5] = 1;
                for (rank_t tmp_rank = 5; tmp_rank <= 7; ++tmp_rank)
                {
                    WMvalue[make_tile(tmp_suit, tmp_rank)] = -1;
                }
            }
        }
    }
    for (tile_t a : hand)
    {
        if (WMvalue[a] >= 0 && WMvalue[a] < 100 && tile_get_suit(a) <= 3)
        {
            if (handCount[a] * handCount[a + 1] * handCount[a + 2])
            {
                for (rank_t i = 1; i <= 9; ++i)
                {
                    WMvalue[make_tile(tile_get_suit(a), i)] = -1;
                }
                WMvalue[a] = 100;
                WMvalue[a + 1] = 100;
                WMvalue[a + 2] = 100;
            }
            else if (handCount[a] >= 3)
            {
                for (rank_t i = 1; i <= 9; ++i)
                {
                    WMvalue[make_tile(tile_get_suit(a), i)] = -1;
                }
                WMvalue[a] = 100;
            }
            else
            {
                WMvalue[a] = handCount[a] * 0.8 + unknown[a] * 0.3 +
                             (handCount[a - 1] + handCount[a + 1]) * 0.9 + (unknown[a - 1] + unknown[a + 1]) * 0.4;
                if (tile_get_rank(a) > 1 && (handCount[a - 1] + unknown[a - 1]) > 0)
                {
                    WMvalue[a] += 0.85 * handCount[a - 2] + 0.35 * handCount[a + 2];
                }
                if (tile_get_rank(a) < 9 && (handCount[a - 1] + unknown[a - 1]) > 0)
                {
                    WMvalue[a] = handCount[a + 2] * 0.85 + unknown[a + 2] * 0.35;
                }
            }
        }
        if (WMvalue[a] >= 0 && WMvalue[a] < 100 && tile_get_suit(a) == TILE_SUIT_HONORS)
        {
            if (handCount[a] >= 3)
            {
                if (tile_get_rank(a) <= 4)
                {
                    for (rank_t i = 1; i <= 4; ++i)
                    {
                        WMvalue[make_tile(TILE_SUIT_HONORS, i)] = -1;
                    }
                    WMvalue[a] = 100;
                }
                else
                {
                    for (rank_t i = 5; i <= 7; ++i)
                    {
                        WMvalue[make_tile(TILE_SUIT_HONORS, i)] = -1;
                    }
                    WMvalue[a] = 100;
                }
            }
            else if (handCount[a] == 2)
            {
                if (tile_get_rank(a) <= 4)
                {
                    if (WUMEN_Count[5])
                    {
                        for (rank_t i = 1; i <= 4; ++i)
                        {
                            WMvalue[make_tile(tile_get_suit(a), i)] = -1;
                        }
                        WMvalue[a] = 100;
                    }
                    else
                    {
                        WMvalue[a] = 90;
                    }
                }
                else
                {
                    if (WUMEN_Count[4])
                    {
                        for (rank_t i = 5; i <= 7; ++i)
                        {
                            WMvalue[make_tile(tile_get_suit(a), i)] = -1;
                        }
                        WMvalue[a] = 100;
                    }
                    else
                    {
                        WMvalue[a] = 90;
                    }
                }
            }
            else if (handCount[a] == 1)
            {
                WMvalue[a] = unknown[a] * 10 + 40;
                if (unknown[a] == 0)
                {
                    WMvalue[a] = 0;
                }
            }
        }
    }
    sort(hand.begin(), hand.end(),
         [](tile_t a, tile_t b) {
             return WMvalue[a] > WMvalue[b];
         });
}

void SortHand_SS()
{
    //改变三同顺中心
    int shunziCount[4][9]; //记录形成以make_tile(tmp_suit,tmp_rank)为中间牌的顺子已有的牌数
    memset(shunziCount, 0, sizeof(shunziCount));
    for (pack_t tmp_pack : mingPai[playerID])
    {
        shunziCount[tile_get_suit(pack_get_tile(tmp_pack))][tile_get_rank(pack_get_tile(tmp_pack))] += 3;
    }
    for (suit_t tmp_suit = 1; tmp_suit <= 3; ++tmp_suit)
    {
        for (rank_t tmp_rank = 2; tmp_rank <= 8; ++tmp_rank)
        {
            shunziCount[tmp_suit][tmp_rank] += (handCount[make_tile(tmp_suit, tmp_rank - 1)] > 0);
            shunziCount[tmp_suit][tmp_rank] += (handCount[make_tile(tmp_suit, tmp_rank)] > 0);
            shunziCount[tmp_suit][tmp_rank] += (handCount[make_tile(tmp_suit, tmp_rank + 1)] > 0);
            /*
            if (unknown[make_tile(tmp_suit, tmp_rank - 1)] + handCount[make_tile(tmp_suit, tmp_rank - 1)] == 0)
            {
                shunziCount[tmp_suit][tmp_rank] = -10;
            }
            if (unknown[make_tile(tmp_suit, tmp_rank)] + handCount[make_tile(tmp_suit, tmp_rank)] == 0)
            {
                shunziCount[tmp_suit][tmp_rank] = -10;
            }
            if (unknown[make_tile(tmp_suit, tmp_rank + 1)] + handCount[make_tile(tmp_suit, tmp_rank + 1)] == 0)
            {
                shunziCount[tmp_suit][tmp_rank] = -10;
            }
            */
        }
    }

    int tmp_shanten = 100, best_shanten = 100;
    double tmp_possibility = 0, best_possibility = 0;
    for (rank_t tmp_rank = 2; tmp_rank <= 8; ++tmp_rank)
    {
        tmp_shanten = 9;
        tmp_shanten -= shunziCount[1][tmp_rank];
        tmp_shanten -= shunziCount[2][tmp_rank];
        tmp_shanten -= shunziCount[3][tmp_rank];
        tmp_possibility = 0;
        for (rank_t cnt_rank = tmp_rank - 2; cnt_rank <= tmp_rank + 2; ++cnt_rank)
        {
            tmp_possibility +=
                unknown[make_tile(1, cnt_rank)] *
                (handCount[make_tile(1, cnt_rank)] == 0) *
                (1 - (double)(cnt_rank - tmp_rank) * (cnt_rank - tmp_rank) / 5);
            tmp_possibility +=
                unknown[make_tile(2, cnt_rank)] *
                (handCount[make_tile(2, cnt_rank)] == 0) *
                (1 - (double)(cnt_rank - tmp_rank) * (cnt_rank - tmp_rank) / 5);
            tmp_possibility +=
                unknown[make_tile(3, cnt_rank)] *
                (handCount[make_tile(3, cnt_rank)] == 0) *
                (1 - (double)(cnt_rank - tmp_rank) * (cnt_rank - tmp_rank) / 5);
        }
        if (tmp_shanten < best_shanten)
        {
            SS_CentralRank = tmp_rank;
            best_shanten = tmp_shanten;
            best_possibility = tmp_possibility;
        }
        else if (tmp_shanten == best_shanten)
        {
            if (tmp_possibility > best_possibility)
            {

                SS_CentralRank = tmp_rank;
                best_shanten = tmp_shanten;
                best_possibility = tmp_possibility;
            }
        }
    }
    if(SS_CentralRank<3)
    {
        ++SS_CentralRank;
    }
    else if(SS_CentralRank>7)
    {
        --SS_CentralRank;
    }
#ifdef _DEBUG_
    cerr <<"Central is"<< (int)SS_CentralRank << endl;
#endif
    for (tile_t a : hand)
    {
        if (tile_get_suit(a) > 3)
        {
            SSvalue[a] = 0;
        }
        else
        {
            if (handCount[a] > 1)
            {
                SSvalue[a] = -1;
            }
            else
            {
                SSvalue[a] = 100 - (tile_get_rank(a) - SS_CentralRank) * (tile_get_rank(a) - SS_CentralRank);
            }
        }
    }
    sort(hand.begin(), hand.end(),
         [](tile_t a, tile_t b) {
             return SSvalue[a] > SSvalue[b];
         });
}

tile_t Discard(vector<tile_t> forbid_ = vector<tile_t>()) //估价弃牌
{
    switch (selfTargetFan)
    {
    case QIDUI:
        SortHand_QIDUI();
        break;
    case WUMENQI:
        SortHand_WUMENQI();
        break;
    case QINGLONG:
        SortHand_QLYS();
        break;
    case YISESANBUGAO:
        SortHand_QLYS();
        break;
    case SANSESANTONGSHUN:
        SortHand_SS();
        break;
    case SANSESANBUGAO:
        SortHand_SS();
        break;
    default:
        break;
    }
    if (forbid_.empty())
    {
        return *(hand.rbegin());
    }
    auto tmp_iter = forbid_.begin();
    bool found = 0;
    for (auto iter = hand.rbegin(); iter != hand.rend(); ++iter)
    {
        found = 0;
        for (tmp_iter = forbid_.begin(); tmp_iter != forbid_.end(); ++tmp_iter)
        {
            if (*tmp_iter == *iter)
            {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            return *iter;
        }
    }
    return *(hand.rbegin());
}

void ChooseTargetFan()
{

    selfTargetFan = SANSESANTONGSHUN;
    return;
    GetHandCount();
    selfTargetFan = QIDUI; //默认为防御力比较高的七对
    int pairCount = 0;
    for (tile_t a : all_tiles)
    {
        if (handCount[a] > 1)
        {
            ++pairCount;
        }
    }
    if (pairCount >= 3)
    {
        selfTargetFan = QIDUI;
#ifdef _DEBUG_
        cerr << "chooseTargetQIDUI" << endl;
#endif
        return;
    }

    int windCount = 0, arrowCount = 0;
    for (rank_t tmp_rank = 1; tmp_rank <= 4; ++tmp_rank)
    {
        windCount = max(windCount, handCount[make_tile(TILE_SUIT_HONORS, tmp_rank)]);
    }
    for (rank_t tmp_rank = 5; tmp_rank <= 7; ++tmp_rank)
    {
        arrowCount = max(arrowCount, handCount[make_tile(TILE_SUIT_HONORS, tmp_rank)]);
    }
    if (windCount * arrowCount >= 2)
    {
        selfTargetFan = WUMENQI;
#ifdef _DEBUG_
        cerr << "chooseTargetWUMENQI" << endl;
#endif
        return;
    }

    int singleColorCount = 0, maxSCC = 0;
    for (suit_t tmp_suit = 1; tmp_suit <= 3; ++tmp_suit)
    {
        singleColorCount = 0;
        for (rank_t tmp_rank = 1; tmp_rank <= 9; ++tmp_rank)
        {
            singleColorCount += (handCount[make_tile(tmp_suit, tmp_rank)] > 0);
        }
        maxSCC = max(singleColorCount, maxSCC);
    }
    if (maxSCC >= 5)
    {
        selfTargetFan = QINGLONG;
#ifdef _DEBUG_
        cerr << "chooseTargetYS" << endl;
#endif
        return;
    }
    //三色类判定
    int shunziCount[4][9]; //记录形成以make_tile(tmp_suit,tmp_rank)为中间牌的顺子已有的牌数
    memset(shunziCount, 0, sizeof(shunziCount));
    for (suit_t tmp_suit = 1; tmp_suit <= 3; ++tmp_suit)
    {
        for (rank_t tmp_rank = 2; tmp_rank <= 8; ++tmp_rank)
        {
            shunziCount[tmp_suit][tmp_rank] += (handCount[make_tile(tmp_suit, tmp_rank - 1)] > 0);
            shunziCount[tmp_suit][tmp_rank] += (handCount[make_tile(tmp_suit, tmp_rank)] > 0);
            shunziCount[tmp_suit][tmp_rank] += (handCount[make_tile(tmp_suit, tmp_rank + 1)] > 0);
        }
    }
    int tmp_shanten = 9, best_shanten = 9;
    rank_t best_MidRank;
    for (rank_t tmp_rank = 2; tmp_rank <= 8; ++tmp_rank)
    {
        tmp_shanten = 9;
        tmp_shanten -= shunziCount[1][tmp_rank];
        tmp_shanten -= shunziCount[2][tmp_rank];
        tmp_shanten -= shunziCount[3][tmp_rank];
        if (tmp_shanten < best_shanten)
        {
            best_MidRank = tmp_rank;
            best_shanten = tmp_shanten;
        }
    }
    if (best_shanten <= 9)
    {
        selfTargetFan = SANSESANTONGSHUN;
#ifdef _DEBUG_
        cerr << "chooseTargetSANSESANTONGSHUN" << endl; //先打三色三同顺，再依据情况更换番形。
#endif
    }
//花龙并不能很好地与三色三同顺一起进行。
#ifdef _DEBUG_
    cerr << "chooseTargetQIDUI" << endl;
#endif
}

int main()
{
    //初始化
    for (int i = 1; i <= 9; i++)
    {
        str2tile["W" + to_string(i)] = mahjong::make_tile(TILE_SUIT_CHARACTERS, i);
        str2tile["B" + to_string(i)] = mahjong::make_tile(TILE_SUIT_DOTS, i);
        str2tile["T" + to_string(i)] = mahjong::make_tile(TILE_SUIT_BAMBOO, i);
    }
    for (int i = 1; i <= 4; i++)
    {
        str2tile["F" + to_string((i))] = mahjong::make_tile(TILE_SUIT_HONORS, i);
    }
    for (int i = 1; i <= 3; i++)
    {
        str2tile["J" + to_string((i))] = mahjong::make_tile(TILE_SUIT_HONORS, i + 4);
    }
    for (tile_t i = 0; i <= 0x50; ++i)
    {
        unknown[i] = 0;
        paiChi[i] = 0;
    }
    for (tile_t i : all_tiles)
    {
        unknown[i] = 4;
        handCount[i] = 0;
    }
#ifdef _MATCH_MODE_
    tileWall[0] = 34;
    tileWall[1] = 34;
    tileWall[2] = 34;
    tileWall[3] = 34;
#endif
    string inputStr;
    Json::Reader myInputReader;
    Json::Value inputJSON;
    int turnID; //用以判定是第几回合
    int type, who, hua;
    string scard, what, act;
    tile_t card;
    //本局输入，进行决策,如果长时运行，前两局可以不读
    for (turnID = 0; turnID < 100000; ++turnID)
    {
        getline(cin, inputStr);
        if (turnID)
        {
            requestsVector.push_back(inputStr);
        }
        else
        {
            myInputReader.parse(inputStr, inputJSON);
            requestsVector.push_back(inputJSON["requests"][0].asString());
        }
        istringstream in(*(requestsVector.rbegin()));
        ostringstream out;
        in >> type;
        switch (type)
        {
        case 0:
            // 告知编号
            in >> playerID >> quan;
            break;
        case 1:
#ifndef _MATCH_MODE_
            for (int i = 0; i < 4; ++i)
            {
                in >> hua;
                tileWall[i] -= hua;
            }
#endif
            for (int j = 0; j < 13; j++)
            {
                in >> scard;
                card = GetTile_t(scard);
                hand.push_back(card);
                --unknown[card];
            }
            tileWall[0] -= 13;
            tileWall[1] -= 13;
            tileWall[2] -= 13;
            tileWall[3] -= 13;
            ChooseTargetFan();
            break;
        case 2:
            // 轮到我摸牌
            in >> scard;
            card = GetTile_t(scard);
            --unknown[card];
            lastDrawCard = card;
            --tileWall[playerID];
            //加入手牌
            xiaJia = (playerID + 1) % 4;
            // 能和则和
            if (Hu(card, 1, 0))
            {
                out << "HU";
            }
            /*else if (CanANGANG(card))
            {

            }
            else if(CanANBUGANG(card))
            {

            }*/
            else
            {
                hand.push_back(card);
                out << "PLAY ";
                card = Discard();
                lastPlay = card;
                DeleteHand(card);
                out << GetStr(card);
            }
            break;
        case 3:

            // 其他玩家信息
            bool takeAct = 0; //是否需要采取行动
            bool tryHu = 0;
            in >> who >> act;

            if (act == "DRAW")
            {
                --tileWall[who];
            }
            if (act == "BUHUA")
            {
                tileWall[who] -= 2;
            }
            if (act == "PLAY")
            {
                xiaJia = (who + 1) % 4;
                takeAct = 1;
                in >> scard;
                card = GetTile_t(scard);
                if (Hu(card, 0, 0))
                {
                    tryHu = 1;
                }
                PlayCard(who, GetTile_t(scard));
            }
            else if (act == "PENG")
            {
                takeAct = 0;
                if (who != playerID)
                {
                    mingPai[who].push_back(make_pack(1, PACK_TYPE_PUNG, lastPlay)); //不管牌的来源
                    unknown[lastPlay] -= 2;
                    in >> scard;
                    card = GetTile_t(scard);
                    PlayCard(who, card);
                    tryHu = Hu(lastPlay, 0, 0);
                }
                if (who == playerID)
                {
                    PENG(lastPlayer, lastPlay);
                    in >> scard;
                    card = GetTile_t(scard);
                    PlayCard(who, card);
                    DeleteHand(card);
                }
            }
            else if (act == "CHI")
            {
                takeAct = 0;
                in >> scard;
                card = GetTile_t(scard);
                if (who != playerID)
                {
                    mingPai[who].push_back(make_pack(1, PACK_TYPE_CHOW, card)); //不管牌的来源
                    --unknown[card - 1];
                    --unknown[card];
                    --unknown[card + 1];
                    in >> scard;
                    card = GetTile_t(scard);
                    PlayCard(who, card);
                    tryHu = Hu(lastPlay, 0, 0);
                }
                else
                {
                    CHI(lastPlay, card);
                    in >> scard;
                    card = GetTile_t(scard);
                    PlayCard(who, card);
                    DeleteHand(card);
                }
            }
            else if (act == "GANG")
            {
                int tmp = WhoDraw(turnID - 1);
                if (tmp == who) //明还是暗
                {
                    if (who == playerID)
                    {
                        ANGANG(lastDrawCard);
                    }
                    else
                    {
                        //没有更新unknown与对方动作
                    }
                }
                else
                {
                    if (who == playerID)
                    {
                        MINGGANG(lastPlayer, lastPlay);
                    }
                    else
                    {
                        mingPai[who].push_back(make_pack(1, PACK_TYPE_KONG, lastPlay));
                        unknown[lastPlay] = 0;
                    }
                }
            }
            else if (act == "BUGANG")
            {
                int tmp = WhoDraw(turnID - 1);
                if (tmp == who) //明还是暗
                {
                    if (who == playerID)
                    {
                        ANBUGANG(lastDrawCard);
                    }
                    else
                    {
                        //没有更新unknown与对方动作
                    }
                }
                /*else
                    {
                        if(who==playerID)
                        {
                            BUGANG(lastPlayer,lastPlay);
                        }
                        else
                        {
                            mingPai[who].push_back(make_pair("GANG",make_pair(GetStr(lastPlay),lastPlayer)));
                        }
                        
                    }
                    */
            }
            if (tryHu)
            {
                takeAct = 0;
                out << "HU";
            }
            if (takeAct)
            {
                tile_t tmp_midcard;
                //在非长时模式下下一局开始时就可以判定是否鸣牌成功
                /*
                else if(CanBUGANG(who,lastPlay))
                {
                    out<<"BUGANG "<<GetStr(lastPlay);
                }
                */
                if (CanMINGGANG(who, lastPlay))
                {
                    out << "GANG " << GetStr(lastPlay);
                }
                else if (CanPENG(who, lastPlay))
                {
                    out << "PENG ";
                    vector<tile_t> formid = {lastPlay};
                    card = Discard(formid);
                    if (card == 0)
                    {
                        out.clear();
                    }
                    else
                    {
                        out << GetStr(card);
                    }
                }
                else if (CanCHI(who, lastPlay, tmp_midcard))
                {
#ifdef _DEBUG_
                    cerr << "goto CHI" << GetStr(tmp_midcard) << endl;
#endif
                    out << "CHI " << GetStr(tmp_midcard) << " ";
                    vector<tile_t> formid = {tmp_midcard - 1, tmp_midcard, tmp_midcard + 1};
                    card = Discard(formid);
                    if (card == 0)
                    {
                        out.clear();
                    }
                    else
                    {
                        out << GetStr(card);
                    }
                }

                /*else if (act == "GANG")
            {
                //暂时不考虑
            }*/
            }
            break;
        }
        if (out.str().empty())
        {
            out << "PASS";
        }

#ifdef _DEBUG_
        for (auto a : hand)
        {
            cerr << "hand" << GetStr(a) << endl;
        }
        for (auto a : mingPai[playerID])
        {
            cerr << "CHI" << PACK_TYPE_CHOW << endl;
            cerr << "PENG" << PACK_TYPE_PUNG << endl;
            cerr << "mingPai " << GetStr(pack_get_tile(a)) << "type=" << (pack_get_type(a) == PACK_TYPE_CHOW) * 1 << endl;
        }
#endif // _DEBUG_获得当下牌组

        Json::Value outputJSON;
        outputJSON["response"] = out.str();
        cout << outputJSON << endl;

        cout << endl
             << ">>>BOTZONE_REQUEST_KEEP_RUNNING<<<" << endl;
        cout << flush;
    }
    return 0;
}
