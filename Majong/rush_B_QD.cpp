#ifndef _BOTZONE_ONLINE
#define _DEBUG_ // 开启Debug模式
#endif
#define _RUSH_B_QD_ //仅使用七对
#define _LONG_TIME_   //使用长时运行
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <unordered_map>
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



vector<pack_t> mingPai[4]; //明牌，字符串化，记录4个人的

int quan; //风圈
// int flowerCount[4];//补花数，对决策无影响,也不计入算番
int playerID; //门风

unordered_map<tile_t,int> handCount;//对手牌中的牌计数，用时更新数据

unordered_map<tile_t, int> unknown; //未知的牌(已知:排池中的牌+明牌+自家暗牌)剩余的数量,用以估价,花牌不考虑

unordered_map<tile_t, int> paiChi; //牌池中的牌，用于分析决张番形

int tileWall = 144; //分析牌墙剩余牌数，计算海底捞月等牌型

tile_t lastPlay; //最后一张打出的手牌编号，用以判定碰杠吃

int lastPlayer; //最后一个出牌人

tile_t lastDrawCard; //自己抽到的最后一张牌

static unordered_map<string, tile_t> str2tile;

inline int GetRelativeID(int player_) //将绝对的排序转换为上下对家
{
    return (playerID + 4 - player_) % 4;
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
    if (tileWall == 0)
    {
        calculate_param.win_flag |= WIN_FLAG_WALL_LAST;
    }
    if (paiChi[winTile_] == 0)
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
#ifdef _RUSH_B_QD_
    return 0;
#endif
    if (who_ == playerID)
    {
        return 0;
    }
    int count = 0;
    for (auto iter = hand.begin(); iter != hand.end(); ++iter)
    {
        count += (*iter == card_);
    }
    return count == 2;
}

void PENG(int who_, tile_t card_) //碰后将牌组从手牌移到明牌
{

    DeleteHand(card_);
    DeleteHand(card_);
    mingPai[playerID].push_back(make_pack(GetRelativeID(who_), PACK_TYPE_PUNG, card_));
}

bool CanMINGGANG(int who_, tile_t card_) //判定能不能明杠,算番库中通过供牌人是否是自己来判定明暗杠
{
#ifdef _RUSH_B_QD_
    return 0;
#endif
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
#ifdef _RUSH_B_QD_
    return 0;
#endif
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
#ifdef _RUSH_B_QD_
    return 0;
#endif
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

tile_t CanCHI(int who_, tile_t card_)
{
#ifdef _RUSH_B_QD_
    return 0;
#endif
    int count[5] = {0, 0, 1, 0, 0}; //分别计数比打出牌-2,-1,1,2的牌
    tile_t midcard = 0;
    if (tile_get_suit(card_) > 3)
    {
        return 0;
    }
    if (GetRelativeID(who_) != 1)
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
            tile_t midcard = card_ - 1 + i;
            return midcard;
        }
    }
    return midcard;
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

void SortHandQD()
{
    //超过两个则优先打出，再按剩余的牌数估计机会
    static unordered_map<tile_t,double> QDvalue;//估价数组
    int unknownSum=0;
    for(tile_t a:all_tiles)
    {
        unknownSum+=unknown[a];
        QDvalue[a]=unknown[a];
        handCount[a]=0;
    }
    for(tile_t a:hand)
    {
        ++handCount[a];
    }
    for(tile_t a:all_tiles)
    {
        if(unknown[a]+handCount[a]<2)
        {
            QDvalue[a]=-1;
        }
        else if(handCount[a]==2)
        {
            QDvalue[a]=100;
        }
        else if(handCount[a]>2)
        {
            QDvalue[a]=0;
        }
        else if(handCount[a]==1)
        {
            QDvalue[a]=((double)unknown[a])/unknownSum;
        }
        else if(handCount[a]==0)
        {
            QDvalue[a]=((double)unknown[a])/unknownSum*((double)unknown[a])/unknownSum*0.9;//延长和牌时间有风险
        }
    }
    sort(hand.begin(),hand.end(),
    [&QDvalue](tile_t a,tile_t b)
    {
        return QDvalue[a]>QDvalue[b];
    }
    );
}

tile_t Discard(vector<tile_t> forbid_ = vector<tile_t>()) //估价弃牌
{
    SortHandQD();
    if (forbid_.empty())
    {
        return *(hand.rbegin());
    }
    auto tmp_iter = forbid_.begin();
    for (auto iter = hand.rbegin(); iter != hand.rend(); ++iter)
    {
        for (tmp_iter = forbid_.begin(); tmp_iter != forbid_.end(); ++tmp_iter)
        {
            if (*tmp_iter == *iter)
                break;
        }
        if (tmp_iter == forbid_.end())
        {
            return *iter;
        }
    }
    return *(hand.rbegin());
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
        handCount[i]=0;
    }

    string inputStr;
    Json::Reader myInputReader;
    Json::Value inputJSON;
    int turnID; //用以判定是第几回合
    int type, who, hua;
    string scard, what, act;
    tile_t card;
//本局输入，进行决策,如果长时运行，前两局可以不读
#ifdef _LONG_TIME_
    turnID = 0;
    for (turnID = 0; turnID < 100000; ++turnID)
#endif
    {

#ifndef _LONG_TIME_
        requestsVector.push_back(inputJSON["requests"][turnID].asString());
#endif

#ifdef _LONG_TIME_
        getline(cin, inputStr);
#ifdef _DEBUG_
        cerr << inputStr << endl;
#endif
        if (turnID)
        {
            requestsVector.push_back(inputStr);
        }
        else
        {
            myInputReader.parse(inputStr, inputJSON);
            requestsVector.push_back(inputJSON["requests"][0].asString());
        }
#endif

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
                tileWall -= hua;
            }
            for (int j = 0; j < 13; j++)
            {
                in >> scard;
                card = GetTile_t(scard);
                hand.push_back(card);
                --unknown[card];
            }
            tileWall -= 4 * 13;
            break;
        case 2:

            // 轮到我摸牌
            in >> scard;
            card = GetTile_t(scard);
            --unknown[card];
            lastDrawCard = card;
            --tileWall;
            //加入手牌

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
                --tileWall;
            }
            if (act == "BUHUA")
            {
                tileWall -= 2;
            }

            if (act == "PLAY")
            {
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
                takeAct = 1;
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
                takeAct = 1;
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

                card = CanCHI(who, lastPlay);
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
                else if (card)
                {
                    out << "CHI " << GetStr(card) << " ";
                    vector<tile_t> formid = {--card, card, ++card};
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
