// #define _DEBUG_     // 开启Debug模式
#define _LONG_TIME_ //使用长时运行
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include "MahjongGB/MahjongGB.h"

#include "jsoncpp/json.h"

using namespace std;
vector<string> responsesVector, requestsVector; //用于储存输入输出,

vector<string> hand; //暗牌,字符串化,已经成组的牌放在最前面

vector<pair<string, pair<string, int>>> mingPai[4]; //明牌，字符串化，记录4个人的

int frozen; //暗牌中已经成组的牌数
int quan;   //风圈
// int flowerCount[4];//补花数，对决策无影响,也不计入算番
int playerID; //门风

int unknown[50]; //未知的牌(已知:排池中的牌+明牌+自家暗牌)剩余的数量,用以估价,花牌不考虑

int paiChi[50]; //牌池中的牌，用于分析决张番形

int tileWall = 144; //分析牌墙剩余牌数，计算海底捞月等牌型

int lastPlay; //最后一张打出的手牌编号，用以判定碰杠吃

int lastPlayer; //最后一个出牌人

string lastDrawCard; //自己抽到的最后一张牌

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

inline string GetStr(const int x_) //给定牌编号，获得相应字符串，花牌只在判断牌墙剩余张数时有用
{
    static char GetStr_head[6] = {'B', 'W', 'T', 'F', 'J', 'H'};
    return string(1, GetStr_head[x_ / 10]) + to_string(x_ % 10);
}

inline int GetInt(const string &s_) //给定字符串，获得相应编号
{
    static char GetInt_head[6] = {'B', 'W', 'T', 'F', 'J', 'H'};
    for (int i = 0; i < 6; ++i)
    {
        if (s_[0] == GetInt_head[i])
        {
            return i * 10 + (s_[1] - '0');
        }
    }
}

bool CmpCardInt(string &s1_, string &s2_) //以牌的编号比较，保证数字牌在前
{
    return GetInt(s1_) < GetInt(s1_);
}

bool CmpCardUnknown(string &s1_, string &s2_) //未知的牌数少，排序中靠后，优先被抛弃
{
    return unknown[GetInt(s1_)] > unknown[GetInt(s1_)];
}

inline void PlayCard(int who_, int s_) //实现将牌放入牌池，消除未知,更新最后打出的牌
{
    lastPlay = s_;
    --unknown[lastPlay];
    ++paiChi[s_];
    lastPlayer = who_;
}

inline void DeleteHand(int s_) //将自己打出的牌从手牌中删除
{
#ifdef _DEBUG_
    cout << "delete" << s_ << endl;
#endif //  _DEBUG_
    string tmp_s = GetStr(s_);
    for (auto iter = hand.begin(); iter != hand.end(); ++iter)
    {
        if (*iter == tmp_s)
        {
            hand.erase(iter);
            return;
        }
    }
}

void SortHand() //将牌成组,目前的方式是每个需要有序手牌的函数都会排序,目前不能用该方法判定CanCHI或CanPENG,因为该牌组可能将牌拆开
{
    sort(hand.begin() + frozen, hand.end(), CmpCardInt);
    for (int i = frozen + 2; i < (int)hand.size(); ++i) //首先实现将刚拿到的牌成组冻结，并放在数组前部
    {
        if ((GetInt(hand[i]) == GetInt(hand[i - 1]) + 1 && GetInt(hand[i]) == GetInt(hand[i - 2]) + 2) || (GetInt(hand[i]) == GetInt(hand[i - 1]) && GetInt(hand[i]) == GetInt(hand[i - 2])))
        {
            swap(hand[frozen], hand[i]);
            swap(hand[frozen + 1], hand[i - 1]);
            swap(hand[frozen + 2], hand[i - 2]);
            frozen += 3;
            i += 2;
        }
    }
    sort(hand.begin() + frozen, hand.end(), CmpCardInt);
    int tmpFrozen = 0; //成对的放在前面
    for (int i = frozen + 1; i < (int)hand.size(); ++i)
    {
        if ((GetInt(hand[i]) == GetInt(hand[i - 1]) + 1 || (GetInt(hand[i]) == GetInt(hand[i - 1]))))
        {
            swap(hand[frozen + tmpFrozen], hand[i]);
            swap(hand[frozen + tmpFrozen + 1], hand[i - 1]);
            tmpFrozen += 2;
            ++i;
        }
    }
    sort(hand.begin() + frozen + tmpFrozen, hand.end(), CmpCardUnknown);
}

inline bool Hu(int winTile_, bool isZIMO_)
{
    /*
    vector<pair<int, string> > MahjongFanCalculator(
    vector<pair<string, pair<string, int> > > pack,
    vector<string> hand,
    string winTile,
    int flowerCount,
    bool isZIMO,
    bool isJUEZHANG,
    bool isGANG,
    bool isLAST,
    int menFeng,
    int quanFeng
);
*/

    vector<pair<int, string>> fan;
    try
    {
        fan =
            MahjongFanCalculator(
                mingPai[playerID],
                hand,
                GetStr(winTile_),
                0,
                isZIMO_,
                0,             // paiChi[GetInt(winTile)]==3,
                0,             //不杠
                tileWall == 0, //不分析
                playerID,
                quan); //花色设定为0，因为其不影响决策
    }
    catch (string &bug) //没赢就算番算番器会throw string类型的error,见MahjongGB.cpp
    {
        return 0;
    }

    int ret = 0;
    for (auto a : fan)
    {
        ret += a.first;
#ifdef _DEBUG_
        cerr << a.second << endl;
#endif
    }
    if (ret >= 8)
    {
        return 1;
    }
    return 0;
    return 1; //因为没抛出bug
}

bool CanPENG(int who_, int card_) //判定能不能碰
{
    if (who_ == playerID)
    {
        return 0;
    }
    int count = 0;
    string tmpCard = GetStr(card_);
    for (auto iter = hand.begin() + frozen; iter != hand.end(); ++iter)
    {
        count += (*iter == tmpCard);
    }
    return count == 2;
}

void PENG(int card_) //碰后将牌组从手牌移到明牌
{

    DeleteHand(card_);
    DeleteHand(card_);
    mingPai[playerID].push_back(make_pair("PENG", make_pair(GetStr(card_), 0)));
}

bool CanMINGGANG(int who_, int card_) //判定能不能明杠,算番库中通过供牌人是否是自己来判定明暗杠
{
    if (who_ == playerID)
    {
        return 0;
    }
    int count = 0;
    string tmpCard = GetStr(card_);
    for (auto iter = hand.begin() + frozen; iter != hand.end(); ++iter)
    {
        count += (*iter == tmpCard);
    }
    return count >= 3;
}

void MINGGANG(int who_, int card_) //明杠
{
    string tmpCard = GetStr(card_);
    for (int i = 0; i < 3; ++i)
    {
        DeleteHand(card_);
    }
    mingPai[playerID].push_back(make_pair("GANG", make_pair(tmpCard, who_))); //如供牌人是自己则为暗杠;
}

bool CanANGANG(int card_) //判定能不能暗杠,算番库中通过供牌人是否是自己来判定明暗杠
{
    int count = 0;
    string tmpCard = GetStr(card_);
    for (auto iter = hand.begin() + frozen; iter != hand.end(); ++iter)
    {
        count += (*iter == tmpCard);
    }
    return count >= 4;
}

void ANGANG(int card_) //暗杠
{
    string tmpCard = GetStr(card_);
    for (int i = 0; i < 4; ++i)
    {
        DeleteHand(card_);
    }
    mingPai[playerID].push_back(make_pair("GANG", make_pair(tmpCard, playerID))); //如供牌人是自己则为暗杠;
}

bool CanBUGANG(int who_, int card_)
{
    if (who_ == playerID)
    {
        return 0;
    }
    for (auto a : mingPai[playerID])
    {
        if (a.first == "PENG")
        {
            if (a.second.first == GetStr(card_))
            {
                return 1;
            }
        }
    }
    return 0;
};

void BUGANG(int who_, int card_)
{
    for (auto a : mingPai[playerID])
    {
        if (a.first == "PENG")
        {
            if (a.second.first == GetStr(card_))
            {
                a.first == "GANG";
                a.second.second = who_;
                return;
            }
        }
    }
}

bool CanANBUGANG(int card_) //暗补杠，但从对局来看，只能补杠自己摸的牌
{
    for (auto a : mingPai[playerID])
    {
        if (a.first == "PENG")
        {
            if (a.second.first == GetStr(card_))
            {
                return 1;
            }
        }
    }
    return 0;
};

void ANBUGANG(int card_) //
{
    for (auto a : mingPai[playerID])
    {
        if (a.first == "PENG")
        {
            if (a.second.first == GetStr(card_))
            {
                a.first == "GANG";
                DeleteHand(card_);
                return;
            }
        }
    }
}

string CanCHI(int who, int card_)
{
    string midcard;
    static int shangJia;
    int tmp;
    shangJia = (playerID - 1 + 4) % 4;
    int count[5] = {0, 0, 1, 0, 0}; //分别计数比打出牌-2,-1,1,2的牌
    if (card_ >= 30)
    {
        return string();
    }
    if (who != shangJia)
    {
        return string();
    }
    for (auto iter = hand.begin() + frozen; iter != hand.end(); ++iter)
    {
        tmp = GetInt(*iter);
        for (int i = 0; i < 5; ++i)
        {
            count[i] += (tmp == card_ - 2 + i);
        }
    }
    //由于不同种牌间的编号有空隙，所以可行
    for (int i = 0; i < 3; ++i)
    {
        if (count[i] && count[i + 1] && count[i + 2])
        {
            midcard = GetStr(card_ - 2 + i + 1);
            return midcard;
        }
    }
    return midcard;
}

void CHI(int card_, int midcard_)
{
    bool waitForDelete[3] = {1, 1, 1};
    waitForDelete[card_ - midcard_ + 1] = 0;
    string tmpCard = GetStr(card_);
    for (int i = 0; i < 3; ++i)
    {
        if (waitForDelete[i])
        {
            DeleteHand(midcard_ - 1 + i);
        }
    }
    mingPai[playerID].push_back(make_pair("CHI", make_pair(GetStr(midcard_), 0)));
}

string Discard(vector<int> forbid_ = vector<int>()) //估价弃牌
{
    SortHand();
    if (forbid_.empty())
    {
        return *(hand.rbegin());
    }
    auto tmp_iter = forbid_.begin();
    for (auto iter = hand.rbegin(); iter != hand.rend() - frozen; ++iter)
    {
        for (tmp_iter = forbid_.begin(); tmp_iter != forbid_.end(); ++tmp_iter)
        {
            if (*tmp_iter == GetInt(*iter))
                break;
        }
        if (tmp_iter == forbid_.end())
        {
            return *iter;
        }
    }
    return string();
}

int main()
{
    //初始化
    MahjongInit(); //算番器初始化
    for (int i = 0; i < 50; ++i)
    {
        unknown[i] = 4;
        paiChi[i] = 0;
    }
    frozen = 0;

    string inputStr;
    Json::Reader myInputReader;
    Json::Value inputJSON;
    int turnID; //用以判定是第几回合

    int type, who, hua;
    string card, what, act;

#ifndef _LONG_TIME_

    getline(cin, inputStr);
    myInputReader.parse(inputStr, inputJSON);
    turnID = inputJSON["responses"].size();

    //输入与更新状态
    {
        for (int i = 0; i < turnID; i++)
        {
            requestsVector.push_back(inputJSON["requests"][i].asString());
            responsesVector.push_back(inputJSON["responses"][i].asString());
            istringstream in(inputJSON["requests"][i].asString()),
                out(inputJSON["responses"][i].asString());

            // 获得信息类型
            in >> type;
            switch (type)
            {
            case 0:
                // 告知编号
                in >> playerID >> quan;
                // response一定是PASS，不用看了
                break;
            case 1:
                for (int i = 0; i < 4; ++i)
                {
                    in >> hua;
                    tileWall -= hua;
                }
                for (int j = 0; j < 13; j++)
                {
                    in >> card;
                    hand.push_back(card);
                    --unknown[GetInt(card)];
                }
                tileWall -= 4 * 13;
                //response一定是PASS，不用看了
                break;
            case 2:
                // 己方回合
                in >> card;
                --tileWall;
                hand.push_back(card);
                lastDrawCard = card;
                --unknown[GetInt(card)];
                out >> act;
                if (act == "PLAY")
                {
                    out >> card;
                    DeleteHand(GetInt(card));
                    lastPlay = GetInt(card);
                }
                else if (act == "GANG") //肯定是暗杠，下一回合读入request后进行操作
                {
                }
                else if (act == "BUGANG") //也一定成功
                {
                }

                break;
            case 3:
                // 别人或自己的动作
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
                    in >> card;
                    PlayCard(who, GetInt(card));
                }
                else if (act == "PENG")
                {
                    if (who != playerID)
                    {
                        mingPai[who].push_back(make_pair("PENG", make_pair(GetStr(lastPlay), 0))); //不管牌的来源
                        unknown[lastPlay] -= 3;
                    }
                    if (who == playerID)
                    {
                        PENG(lastPlay);
                    }

                    in >> card;
                    PlayCard(who, GetInt(card));
                    if (who == playerID)
                    {
                        DeleteHand(GetInt(card));
                    }
                }
                else if (act == "CHI")
                {
                    in >> card;
                    if (who != playerID)
                    {
                        mingPai[who].push_back(make_pair("CHI", make_pair(card, 0))); //不管牌的来源
                        --unknown[GetInt(card) - 1];
                        --unknown[GetInt(card)];
                        --unknown[GetInt(card) + 1];
                    }
                    else
                    {
                        CHI(lastPlay, GetInt(card));
                    }
                    in >> card;
                    PlayCard(who, GetInt(card));
                    if (who == playerID)
                    {
                        DeleteHand(GetInt(card));
                    }
                }
                else if (act == "GANG")
                {
                    int tmp = WhoDraw(i - 1);
                    if (tmp == who) //明还是暗
                    {
                        if (who == playerID)
                        {

                            ANGANG(GetInt(lastDrawCard));
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
                            mingPai[who].push_back(make_pair("GANG", make_pair(GetStr(lastPlay), lastPlayer)));
                            unknown[lastPlay] -= 3; //只有3张未知
                        }
                    }
                }
                else if (act == "BUGANG")
                {
                    int tmp = WhoDraw(i - 1);
                    if (tmp == who) //明还是暗
                    {
                        if (who == playerID)
                        {

                            ANBUGANG(GetInt(lastDrawCard));
                        }
                        else
                        {
                            //没有更新unknown与对方动作
                        }
                    }
                    /*
                    else
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
                out >> act;

                if (act == "PENG")
                {
                }
                else if (act == "CHI")
                {
                }
                else if (act == "GANG")
                {
                    //暂时不杠也不补杠
                }
            }
        }
    }
#endif
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
        cerr<<inputStr<<endl;
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
                in >> card;
                hand.push_back(card);
                --unknown[GetInt(card)];
            }
            tileWall -= 4 * 13;
            break;
        case 2:

            // 轮到我摸牌
            in >> card;
            --unknown[GetInt(card)];
            lastDrawCard = card;
            --tileWall;
            //加入手牌

            // 能和则和
            if (Hu(GetInt(card), 1))
            {
                out << "HU";
            }
            else
            {
                hand.push_back(card);
                out << "PLAY ";
                card = Discard();
                lastPlay = GetInt(card);
                DeleteHand(GetInt(card));
                out << card;
            }
            break;
        case 3:

            // 其他玩家信息
            bool takeAct = 0; //是否需要采取行动
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
                in >> card;
                PlayCard(who, GetInt(card));
            }
            else if (act == "PENG")
            {
                takeAct = 1;
                if (who != playerID)
                {
                    mingPai[who].push_back(make_pair("PENG", make_pair(GetStr(lastPlay), 0))); //不管牌的来源
                    unknown[lastPlay] -= 3;
                }
                if (who == playerID)
                {
                    PENG(lastPlay);
                }

                in >> card;
                PlayCard(who, GetInt(card));
                DeleteHand(GetInt(card));
            }
            else if (act == "CHI")
            {
                takeAct = 1;
                in >> card;
                if (who != playerID)
                {
                    mingPai[who].push_back(make_pair("CHI", make_pair(card, 0))); //不管牌的来源
                    --unknown[GetInt(card) - 1];
                    --unknown[GetInt(card)];
                    --unknown[GetInt(card) + 1];
                }
                else
                {
                    CHI(lastPlay, GetInt(card));
                }
                in >> card;
                PlayCard(who, GetInt(card));
                DeleteHand(GetInt(card));
            }
            else if (act == "GANG")
            {
                int tmp = WhoDraw(turnID - 1);
                if (tmp == who) //明还是暗
                {
                    if (who == playerID)
                    {

                        ANGANG(GetInt(lastDrawCard));
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
                        mingPai[who].push_back(make_pair("GANG", make_pair(GetStr(lastPlay), lastPlayer)));
                        unknown[lastPlay] -= 3; //只有3张未知
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

                        ANBUGANG(GetInt(lastDrawCard));
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
            if (takeAct)
            {

                card = CanCHI(who, lastPlay);
                if (Hu(lastPlay, 0))
                {
                    out << "HU";
                }
                //在非长时模式下下一局开始时就可以判定是否鸣牌成功
                /*
                else if(CanBUGANG(who,lastPlay))
                {
                    out<<"BUGANG "<<GetStr(lastPlay);
                }
                */
                else if (CanMINGGANG(who, lastPlay))
                {
                    out << "GANG " << GetStr(lastPlay);
                }
                else if (CanPENG(who, lastPlay))
                {
                    out << "PENG ";
                    vector<int> formid = {lastPlay};
                    card = Discard(formid);
                    if (card.empty())
                    {
                        out.clear();
                    }
                    else
                    {
                        out << card;
                    }
                }
                else if (!card.empty())
                {
                    out << "CHI " << card << " ";
                    vector<int> formid = {GetInt(card) - 1, GetInt(card), GetInt(card) + 1};
                    card = Discard(formid);
                    if (card.empty())
                    {
                        out.clear();
                    }
                    else
                    {
                        out << card;
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
            cerr << "hand" << a << endl;
        }
        for (auto a : mingPai[playerID])
        {
            cerr << "mingPai " << a.first << a.second.first << endl;
        }
#endif // _DEBUG_获得当下牌组

        Json::Value outputJSON;
        outputJSON["response"] = out.str();
        cout << outputJSON << endl;

#ifdef _DEBUG_
        getchar();
#endif
#ifdef _LONG_TIME_
        cout << endl
             << ">>>BOTZONE_REQUEST_KEEP_RUNNING<<<" << endl;
        cout << flush;
#endif
    }
    return 0;
}
