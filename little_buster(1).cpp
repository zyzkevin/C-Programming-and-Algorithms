#ifndef _BOTZONE_ONLINE
#define _DEBUG_ // 开启Debug模式
#endif
#define _MATCH_MODE_ //新赛制
#define _LONG_TIME_  //使用长时运行
//#define _GREEDY_
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

typedef int (*shantenFunction)(const tile_t *, intptr_t, useful_table_t *);

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

int xiaJia = 1; //最后一个出牌人的下家

tile_t lastDrawCard; //自己抽到的最后一张牌

static unordered_map<string, tile_t> str2tile;

enum
{
    BASICFORM,
    QIDUI,
    ZUHELONG,
    QUANBUKAO,
    SHISANYAO
};                                  //特殊和型：七对，组合龙，全不靠，十三幺
const int MAXTARGETFAN = SHISANYAO; //遍历番种的上限

bool QUANQIUREN = 0; //是否走全求人路线

map<int, double>
    fanValue; //对番种进行估价

int selfTargetFan = BASICFORM;

unordered_map<tile_t, double> handValue;

void SortHand(shantenFunction f);

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

inline shantenFunction ChooseShantenFunction(int targetFan_) //返回与番形对应的上听函数
{
    switch (targetFan_)
    {
    case BASICFORM:
        return basic_form_shanten;
        break;
    case QIDUI:
        return seven_pairs_shanten;
        break;
    case SHISANYAO:
        return thirteen_orphans_shanten;
        break;
    case ZUHELONG:
        return knitted_straight_shanten;
        break;
    case QUANBUKAO:
        return honors_and_knitted_tiles_shanten;
        break;
    default:
        break;
    }
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
#ifdef _LB_DBG_
    cerr << GetStr(s_) << "PLAYED" << paiChi[s_];
#endif
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
#ifdef _DEBUG_
    cerr << "Fan=" << re << '\n';
    for (int i = 0; i < mahjong::FAN_TABLE_SIZE; i++)
    {
        if (fan_table[i] > 0)
        {
            cerr << fan_table[i] * fan_value_table[i] << ' ' << fan_name[i] << '\n';
        }
    }
#endif
    if (re > 0 && !(mingPai[playerID].empty())) //当前牌型不能到8番
    {
        QUANQIUREN = 1;
    }
    return re >= 8;
}

bool CanPENG(int who_, tile_t card_) //判定能不能碰
{
    if (who_ == playerID)
    {
        return 0;
    }
    if (TileWallOut())
    {
        return 0;
    }
    int count = 0;
    for (auto iter = hand.begin(); iter != hand.end(); ++iter)
    {
        count += (*iter == card_);
    }
    bool ret = (count == 2);
    //分析碰对番形是否有利
    if (ret && selfTargetFan != BASICFORM)
    {
        ret = 0;
    }
#ifndef _GREEDY_
    if (!QUANQIUREN)
    {
        if (ret)
        {
            int shanten_before = basic_form_shanten(&hand[0], hand.size(), NULL);
            //假设吃了，计算上听数.因为必定会减小听牌面，因此必须增加上听数,此处可能超时
            hand.push_back(card_);
            SortHand(basic_form_shanten);
            int shanten_after = basic_form_shanten(&hand[0], hand.size() - 1, NULL);
            DeleteHand(card_);
#ifdef _DEBUG_
            cerr << "PENG Shanten Change" << shanten_before << "vs" << shanten_after << '\n';
#endif

            ret = (shanten_after < shanten_before);
        }
    }
#endif

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
    if (selfTargetFan != BASICFORM)
    {
        return 0;
    }
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
    bool ret = count >= 3;
    /*if (ret)
    {
        int shanten_before = basic_form_shanten(&hand[0], hand.size(), NULL);
        //假设吃了，计算上听数,因为必定会减小听牌面，因此必须增加上听数,此处可能超时
        hand.push_back(card_);
        int shanten_after = basic_form_shanten(&hand[0], hand.size(), NULL);
#ifdef _DEBUG_
        cerr << "GANG Shanten Change" << shanten_before << "vs" << shanten_after << '\n';
#endif
        DeleteHand(card_);
        ret = (shanten_after < shanten_before);
    }
    */
    return ret;
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
    if (selfTargetFan != BASICFORM)
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
    mingPai[playerID].push_back(make_pack(0, PACK_TYPE_KONG, card_));
}

bool CanANBUGANG(tile_t card_) //暗补杠，但从对局来看，只能补杠自己摸的牌
{
    if (selfTargetFan != BASICFORM)
    {
        return 0;
    }
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
    if (TileWallOut())
    {
        return 0;
    }
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
            break;
        }
    }
    if (selfTargetFan != BASICFORM)
    {
        ret = 0;
    }
#ifndef _GREEDY_
    if (!QUANQIUREN)
    {
        if (ret)
        {
            int shanten_before = basic_form_shanten(&hand[0], hand.size(), NULL);
            //假设吃了，计算上听数,因为必定会减小听牌面，因此必须增加上听数,此处可能超时
            hand.push_back(card_);
            SortHand(basic_form_shanten);
            int shanten_after = basic_form_shanten(&hand[0], hand.size() - 1, NULL);
#ifdef _DEBUG_
            cerr << "CHI Shanten Change" << shanten_before << "vs" << shanten_after << '\n';
#endif
            DeleteHand(card_);
            ret = (shanten_after < shanten_before);
        }
    }
#endif

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

void SortHand(shantenFunction f)
{
#ifndef _LB_DBG_
    random_shuffle(hand.begin(), hand.end());
#endif
    useful_table_t useful_table;
    for (auto iter = hand.begin(); iter != hand.end(); ++iter)
    {
        handValue[*iter] = 0;
        swap(*iter, *(hand.rbegin()));
        memset(useful_table, 0, sizeof(useful_table));
        int tmp_shanten = f(&hand[0], hand.size() - 1, &useful_table);
        swap(*iter, *(hand.rbegin()));
        double tmp_value = 0;
        for (tile_t a : all_tiles)
        {
#ifndef _LB_DBG_
            if (unknown[a] < 0)
            {
                unknown[a] = 0;
            }
#endif
            if (useful_table[a])
            {
                tmp_value+=unknown[a];
                if(tile_get_suit(a)<=3 && unknown[a])
                {
                    tmp_value+=0.01*(tile_get_rank(a)-1)*(9-tile_get_rank(a));
                }
            }
        }
        tmp_value -= tmp_shanten;
        if (tmp_value < 0)
        {
            tmp_value = -100;
        }
        tmp_value /= 10 + 4 * (tmp_shanten + 1) * (tmp_shanten + 1);
        tmp_value += 14 - tmp_shanten;
        handValue[*iter] = -tmp_value; //弃去后价值越高，此手牌价值越低
    }
#ifdef _DEBUG_
    /*for (auto a : hand)
    {
        cerr << "handValue" << GetStr(a) << ' ' << handValue[a] << '\n';
    }*/
#endif
    sort(hand.begin(), hand.end(),
         [](tile_t a, tile_t b) {
             return handValue[a] > handValue[b];
         });
}

void ChooseTargetFan()
{
    selfTargetFan = BASICFORM;      //默认基本和型
    if (!mingPai[playerID].empty()) //不门清就不能特殊和型
    {
        return;
    }
    double tmp_value;
    int tmp_shanten, tmp_target;
    useful_table_t tmp_usefulTable;
    shantenFunction tmp_shantenFunction;
    for (tmp_target = 0; tmp_target <= MAXTARGETFAN; ++tmp_target)
    {
        tmp_shantenFunction = ChooseShantenFunction(tmp_target);
        tmp_value = 0;
        memset(tmp_usefulTable, 0, sizeof(tmp_usefulTable));
        SortHand(tmp_shantenFunction);
        tmp_shanten = tmp_shantenFunction(&hand[0], 13 - 3 * mingPai[playerID].size(), &tmp_usefulTable);
        for (tile_t a : all_tiles)
        {
            if (tmp_usefulTable[a])
            {
                tmp_value += unknown[a];
            }
        }
#ifdef _DEBUG_
        cerr << "effective card for Fan" << tmp_target << ' ' << tmp_value << endl;
        cerr << "shanten for Fan" << tmp_target << ' ' << tmp_shanten << endl;
#endif
        tmp_value /= 144;
        tmp_value -= tmp_shanten;
        fanValue[tmp_target] = tmp_value;
    }
    fanValue[QIDUI] += 1.2;
    fanValue[BASICFORM] += 1;
    double maxValue = fanValue[BASICFORM];
    for (int i = 1; i <= MAXTARGETFAN; ++i)
    {
        if (fanValue[i] > maxValue)
        {
            selfTargetFan = i;
        }
    }
#ifdef _DEBUG_
    cerr << "chooseFan" << (int)selfTargetFan << endl;
#endif
}

tile_t Discard(vector<tile_t> forbid_ = vector<tile_t>(), int oper = 0) //估价弃牌
{
    ChooseTargetFan();
    SortHand(ChooseShantenFunction(selfTargetFan));
    if (forbid_.empty())
    {
        return *(hand.rbegin());
    }
    auto tmp_iter = forbid_.begin();
    bool found = 0;
    GetHandCount();
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
        else if (oper == 1)
        {
            if (handCount[*iter] > 1)
            {
                return *iter;
            }
        }
    }
    return 0;
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
            for (int i = 0; i < 4; ++i)
            {
                in >> hua;
                tileWall[i] -= hua;
            }

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
            else if (CanANGANG(card))
            {
                out << "GANG";
            }
            else if (CanANBUGANG(card))
            {
                out << "BUGANG " << GetStr(card);
                ANBUGANG(card);
            }
            else
            {
                hand.push_back(card);
                out << "PLAY ";
                card = Discard();
                lastPlay = card;
                DeleteHand(card);
                out << GetStr(card); //消除位置放在下一回合输入play信息
                paiChi[card] += 1;
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
            else if (act == "PLAY")
            {
                xiaJia = (who + 1) % 4;
                takeAct = (who != playerID);
                in >> scard;
                card = GetTile_t(scard);
                if (who != playerID)
                {
                    if (Hu(card, 0, 0))
                    {
                        tryHu = 1;
                    }
                    PlayCard(who, card); //如果不是自己，摸牌时已经消除未知
                }
            }
            else if (act == "PENG")
            {
                takeAct = 0;
                in >> scard;
                card = GetTile_t(scard);
                paiChi[lastPlay] -= 1; //将被鸣的牌从牌池中取出
#ifdef _LB_DBG_
                cerr << "Peng cause paiCHI-" << paiChi[GetTile_t("B8")] << endl;
#endif
                if (who != playerID)
                {
                    mingPai[who].push_back(make_pack(1, PACK_TYPE_PUNG, lastPlay)); //不管牌的来源
                    unknown[lastPlay] -= 2;
                    PlayCard(who, card);
                    tryHu = Hu(card, 0, 0);
                }
                else if (who == playerID)
                {
                    PENG(lastPlayer, lastPlay);
                    PlayCard(who, card);
                    DeleteHand(card);
                }
            }
            else if (act == "CHI")
            {
                takeAct = 0;
                in >> scard;
                card = GetTile_t(scard);
                paiChi[lastPlay] -= 1; //将被鸣的牌从牌池中取出
#ifdef _LB_DBG_
                cerr << "Peng cause paiCHI-" << paiChi[GetTile_t("B8")] << endl;
#endif
                if (who != playerID)
                {
                    mingPai[who].push_back(make_pack(1, PACK_TYPE_CHOW, card)); //不管牌的来源
                    --unknown[card - 1];
                    --unknown[card];
                    --unknown[card + 1];
                    ++unknown[lastPlay]; //吃掉的牌已经在被打出时减去
                    in >> scard;
                    card = GetTile_t(scard);
                    PlayCard(who, card);
                    tryHu = Hu(card, 0, 0);
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
#ifdef _LB_DBG_
                    cerr << "GANG cause paiCHI-" << paiChi[GetTile_t("B8")] << endl;
#endif
                    paiChi[lastPlay] -= 1;
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
                in >> scard;
                tile_t BUGANGcard = GetTile_t(scard);
                if (who == playerID)
                {
                    ANBUGANG(BUGANGcard);
                }
                else
                {
                    unknown[BUGANGcard] = 0;
                }
            }
            if (tryHu)
            {
                takeAct = 0;
                out << "HU";
            }
            if (takeAct)
            {
                tile_t tmp_midcard;
                if (CanMINGGANG(who, lastPlay))
                {
                    out << "GANG";
                }
                else if (CanPENG(who, lastPlay))
                {
                    out << "PENG ";
                    vector<tile_t> formid = {lastPlay};
                    card = Discard(formid, 2);
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
                    card = Discard(formid, 1);
                    if (card == 0)
                    {
                        out.clear();
                    }
                    else
                    {
                        out << GetStr(card);
                    }
                }
            }
            break;
        }
        if (out.str().empty())
        {
            out << "PASS";
        }

#ifdef _DEBUG_
        /*for (auto a : hand)
        {
            cerr << "hand" << GetStr(a) << endl;
        }
        for (auto a : mingPai[playerID])
        {
            cerr << "CHI" << PACK_TYPE_CHOW << endl;
            cerr << "PENG" << PACK_TYPE_PUNG << endl;
            cerr << "mingPai " << GetStr(pack_get_tile(a)) << "type=" << (pack_get_type(a) == PACK_TYPE_CHOW) * 1 << endl;
        }
        cerr << "XiaJia" << xiaJia << endl;
        */
        for (auto a : all_tiles)
        {
            cerr << "Unknown" << GetStr(a) << ' ' << unknown[a] << endl;
        }
        /*
        for (int i = 0; i <= MAXTARGETFAN; ++i)
        {
            cerr << "Fan" << i << " Value" << fanValue[i] << endl;
        }
        */

        for (auto a : all_tiles)
        {
            cerr << "PaiChi:" << GetStr(a) << ' ' << paiChi[a] << endl;
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