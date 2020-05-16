#define _DEBUG_
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include "MahjongGB/MahjongGB.h"

#include "jsoncpp/json.h"

//使用长时运行#define _LONG_TIME_

//#define SIMPLEIO 0
//由玩家自己定义，0表示JSON交互，1表示简单交互。
using namespace std;
vector<string> response, request;

vector<string> hand; //暗牌,字符串化,已经成组的牌放在最前面

vector<pair<string, pair<string, int>>> mingPai[4]; //明牌，字符串化，记录4个人的

int frozen; //暗牌中已经成组的牌数
int quan;   //风圈
// int flowerCount[4];//补花数，对决策无影响,也不计入算番
int playerID; //门风

int unknown[50]; //未知的牌剩余的数量,用以估价,花牌不考虑

int tileWall = 144; //分析牌墙剩余牌数，计算海底捞月等牌型

int lastPlay; //最后一张打出的手牌编号，用以判定碰杠

bool waitForCHI, waitForPENG;

inline string Getstr(const int x_) //给定牌编号，获得相应字符串
{
    static char Getstr_head[6] = {'B', 'W', 'T', 'F', 'J', 'H'};
    return string(1, Getstr_head[x_ / 10]) + to_string(x_ % 10);
}

inline int Getint(const string &s_) //给定字符串，获得相应编号
{
    static char Getint_head[6] = {'B', 'W', 'T', 'F', 'J', 'H'};
    for (int i = 0; i < 6; ++i)
    {
        if (s_[0] == Getint_head[i])
        {
            return i * 10 + (s_[1] - '0');
        }
    }
}

bool CmpCardInt(string& s1_, string& s2_)
{
    return Getint(s1_) < Getint(s1_);
}

inline void Playcard(string &s_)
{

    lastPlay = Getint(s_);
    --unknown[lastPlay];
}

inline void Deletehand(const string &s_)
{
#ifdef  _DEBUG_
    cout <<"delete"<<s_ << endl;
#endif //  _DEBUG_

    for (auto iter = hand.begin(); iter != hand.end(); ++iter)
    {
        if (*iter == s_)
        {
            hand.erase(iter);
            return;
        }
    }
}

inline bool Hu(const string &winTile_, bool isZIMO_)
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
    int ret = 0;
    vector<pair<int, string> > fan;
    try
    {
        fan =
            MahjongFanCalculator(
                mingPai[playerID],
                hand,
                winTile_,
                0,
                isZIMO_,
                0, // paichi[Getint(winTile_)]==0,可以判定
                0, //不杠
                tileWall==0, //不分析
                playerID,
                quan); //花色设定为0，因为其不影响决策
    }
    catch(string & bg)
    {
        return 0;
    }

    for (auto a : fan)
    {
        ret += a.first;
    }
    if (ret >= 8)
    {
        return 1;
    }
    return 0;
}

bool CanPENG(int who_,int card_)
{
    if (who_ == playerID)
    {
        return 0;
    }
    int count = 0;
    string tmpCard = Getstr(card_);
    for (auto iter = hand.begin() + frozen; iter != hand.end(); ++iter)
    {
        count += (*iter == tmpCard);
    }
    return count == 2;
}

void PENG(int card_)
{
    string tmpCard = Getstr(card_);
    Deletehand(tmpCard);
    Deletehand(tmpCard);
    mingPai[playerID].push_back(make_pair("PENG", make_pair(tmpCard, 0)));
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
        tmp = Getint(*iter);
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
            midcard = Getstr(card_ - 2 + i + 1);
            return midcard;
        }
    }
    return midcard;
}

void CHI(int card_, int midcard_)
{
    bool waitToFind[3] = {1, 1, 1};
    waitToFind[card_ - midcard_+1] = 0;
    string tmpCard = Getstr(card_);
    for (int i = 0;i < 3;++i)
    {
        if (waitToFind[i])
        {
            Deletehand(Getstr(midcard_ - 1 + i));
        }
    }
    mingPai[playerID].push_back(make_pair("CHI", make_pair(Getstr(midcard_), 0)));
}

string Discard(vector<int> forbid_=vector<int>()) //估价弃牌
{
    //首先实现将刚拿到的牌成组冻结
    sort(hand.begin() + frozen, hand.end(),CmpCardInt);
    for (int i = frozen + 2; i <(int)hand.size(); ++i)
    {
        if ((Getint(hand[i]) == Getint(hand[i - 1]) + 1 && Getint(hand[i]) == Getint(hand[i - 2]) + 2) || (Getint(hand[i]) == Getint(hand[i - 1]) && Getint(hand[i]) == Getint(hand[i - 2])))
        {
            swap(hand[frozen], hand[i]);
            swap(hand[frozen + 1], hand[i - 1]);
            swap(hand[frozen + 2], hand[i - 2]);
            frozen += 3;
            i += 2;
        }
    }
    sort(hand.begin() + frozen, hand.end(), CmpCardInt);
    int tmpFrozen=0;//成对的放在前面
    for (int i = frozen + 1; i < (int)hand.size(); ++i)
    {
        if ((Getint(hand[i]) == Getint(hand[i - 1]) + 1  || (Getint(hand[i]) == Getint(hand[i - 1]) )))
        {
            swap(hand[frozen+tmpFrozen], hand[i]);
            swap(hand[frozen + tmpFrozen+1], hand[i - 1]);
            tmpFrozen += 2;
            ++i;
        }
    }
    sort(hand.begin()+frozen+tmpFrozen, hand.end(), CmpCardInt);
    if (forbid_.empty())
    {
        return *(hand.rbegin());
    }
    auto tmp_iter = forbid_.begin();
    for (auto iter = hand.rbegin();iter != hand.rend()-frozen;++iter)
    {
        for (tmp_iter = forbid_.begin();tmp_iter != forbid_.end();++tmp_iter)
        {
            if (*tmp_iter == Getint(*iter))break;
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
    int turnID; //用以判定是第几回合
    Json::Value inputJSON;
    cin >> inputJSON;
    turnID = inputJSON["responses"].size();
    //初始化
    MahjongInit();
    for (int i = 0; i < 50; ++i)
    {
        unknown[i] = 4;
    }
    frozen = 0;
#ifndef _LONG_TIME_

    //输入与更新状态
    {
        for (int i = 0; i < turnID; i++)
        {
            istringstream in(inputJSON["requests"][i].asString()),
                out(inputJSON["responses"][i].asString());
            string act;

            int type, who, hua;
            string card, what;

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
                    --unknown[Getint(card)];
                }
                tileWall -= 4 * 13;
                //response一定是PASS，不用看了
                break;
            case 2:
                // 己方回合
                in >> card;
                --tileWall;
                hand.push_back(card);
                --unknown[Getint(card)];
                out >> act;
                if (act == "PLAY")
                {
                    out >> card;
                    Deletehand(card);
                    lastPlay = Getint(card);
                }
                else if (act == "GANG")
                {
                    //暂时不考虑自己杠
                }
                else if (act == "BUGANG")
                {
                    //也不考虑补杠
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
                    Playcard(card);
                }
                else if (act == "PENG")
                {
                    if (who != playerID)
                    {
                        mingPai[who].push_back(make_pair("PENG", make_pair(Getstr(lastPlay), 0))); //不管牌的来源
                        unknown[lastPlay] -= 3;
                    }
                    if (who == playerID)
                    {
                        PENG(lastPlay);
                    }

                    in >> card;
                    Playcard(card);
                    if (who == playerID)
                    {
                        Deletehand(card);
                    }
                    
                }
                else if (act == "CHI")
                {
                    in >> card;
                    if (who != playerID)
                    {
                        mingPai[who].push_back(make_pair("CHI", make_pair(card, 0))); //不管牌的来源
                        --unknown[Getint(card) - 1];
                        --unknown[Getint(card)];
                        --unknown[Getint(card) + 1];
                    }
                    else
                    {
                        CHI(lastPlay, Getint(card));
                    }
                    in >> card;
                    Playcard(card);
                    if (who == playerID)
                    {
                        Deletehand(card);
                    }
                }
                else if (act == "GANG")
                {
                    //暂时不考虑
                }
                //BUGANG对未知牌池没有影响

                //自己行为,CHI,PENG,GANG均在下一局判定
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
    istringstream in(inputJSON["requests"][turnID].asString());
    ostringstream out;
    int type, who;
    string act, card, temp;
    in >> type;
    if (type == 2)
    {
        // 轮到我摸牌
        in >> card;
        --unknown[Getint(card)];

        //加入手牌

        // 能和则和
        if (Hu(card, 1))
        {
            out << "HU";
        }
        else
        {
            hand.push_back(card);
            out << "PLAY ";
            card = Discard();
            waitForCHI = 1;
            lastPlay = Getint(card);
            Deletehand(card);
            out << card;
        }
    }
    else if (type == 3)
    {
        // 其他玩家信息
        bool takeAct = 0;//是否需要采取行动
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
            Playcard(card);
        }
        else if (act == "PENG")
        {
            takeAct = 1;
            if (who != playerID)
            {
                mingPai[who].push_back(make_pair("PENG", make_pair(Getstr(lastPlay), 0))); //不管牌的来源
                unknown[lastPlay] -= 3;
            }
            if (who == playerID)
            {
                PENG(lastPlay);
            }

            in >> card;
            Playcard(card);
            Deletehand(card);
        }
        else if (act == "CHI")
        {
            takeAct = 1;
            in >> card;
            if (who != playerID)
            {
                mingPai[who].push_back(make_pair("CHI", make_pair(card, 0))); //不管牌的来源
                --unknown[Getint(card) - 1];
                --unknown[Getint(card)];
                --unknown[Getint(card) + 1];
            }
            else
            {
                CHI(lastPlay, Getint(card));
            }
            in >> card;
            Playcard(card);
            Deletehand(card);
        }
        else if (act == "GANG")
        {
            //暂时不考虑
        }
        //对其他玩家打出的手牌判定是否点炮>碰>吃
        //由于下一回合输入才能判定，这里先不删除手牌，也不改变lastPlay
        if (takeAct)
        {

            card = CanCHI(who, lastPlay);
            if (Hu(Getstr(lastPlay), 0))
            {
                out << "HU";
            }
            //在非长时模式下下一局开始时就可以判定是否鸣牌成功
            else if (CanPENG(who,lastPlay))
            {
                out << "PENG ";
                vector<int> formid = {lastPlay };
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
                vector<int> formid = { Getint(card) - 1,Getint(card),Getint(card) + 1 };
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
    }
    if(out.str().empty())
    {
        out<<"PASS";
    }
#ifdef _DEBUG_
    for (auto a : hand)
    {
        cout << a << endl;
    }
#endif // _DEBUG_

    Json::Value outputJSON;
    outputJSON["response"] = out.str();
    cout << outputJSON<< endl;
#ifdef _LONG_TIME_
    cout << endl
         << ">>>BOTZONE_REQUEST_KEEP_RUNNING<<<" << endl;
    cout << flush;
#endif
    return 0;
}
