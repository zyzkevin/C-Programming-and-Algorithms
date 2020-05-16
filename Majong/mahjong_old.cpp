// Mahjong from 2z
// 贪心+搜索+估价
// 作者：GDEZOI
// 游戏信息：http://www.botzone.org/games#Mahjong

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include "jsoncpp/json.h"

using namespace std;

#define min(a, b) ((a) < (b) ? (a) : (b))

#define abs(a) ((a) > 0 ? (a) : (-(a)))

int myID, cardCount, nonFrozenCount, cardCount_2, cnt, lastcnt, lastnx;
string cards[108], cards_2[108];
int LiuJu[3][10];

int paichi[30]; //未出现的牌剩余的数量
/*
 * [01]1B
 * [11]1T
 * [21]1W
 * 表示剩余数量
 */

char chr (int i) // 把 a 转成字符 a为0~9的数字
{
	char x = i + 48;
	return x;
}

//把数字变成饼条万
string Getstr(int x)
{
	string s = "";
	switch (x / 10)
	{
		case 0:
			s = "B";
			break;
		case 1:
			s = "T";
			break;
		case 2:
			s = "W";
			break;
	}
	s += chr(x % 10);
	return s;
}

// 对牌排序，获得可打出牌的数量
void SortAndGetFrozenCount()
{
	sort(cards, cards + cardCount);
	for (int i = 0; i < cardCount; i++)
	{
		if (cards[i][0] >= 'a')
		{
			nonFrozenCount = i;
			return;
		}
	}
	nonFrozenCount = cardCount;
}

// 递归检查能否胡
bool CheckHu(int num[10])
{
	for (int i = 1; i <= 9; i++)
		if (num[i] < 0)
			return false;
	if (num[0] == 0)
		return true;
	if (num[0] == 1)
		return false;
	if (num[0] == 2)
	{
		// 剩下两张将牌
		for (int i = 1; i <= 9; i++)
			if (num[i] == 2)
				return true;
		return false;
	}

	for (int i = 1; i <= 9; i++)
	{
		if (num[i] > 0) 
		{
			if (i <= 7)
			{
				// ABC型句子
				num[i]--, num[i + 1]--, num[i + 2]--;
				num[0] = num[0] - 3;
				if (CheckHu(num))
					return true;
				num[0] = num[0] + 3;
				num[i]++, num[i + 1]++, num[i + 2]++;
			}
			if (num[i] >= 3)
			{
				// AAA型句子
				num[i] = num[i] - 3;
				num[0] = num[0] - 3;
				if (CheckHu(num))
					return true;
				num[0] = num[0] + 3;
				num[i] = num[i] + 3;
			}
		}
	}

	return false;
}

// 判断能不能胡
bool Hu()
{
	SortAndGetFrozenCount();
	if (cardCount < 14 || cardCount > 18)
		return false;
	if ((nonFrozenCount - 2) % 3 != 0)
		return false;
	int num[3][10] = { 0 }; // 顺序：万、筒、条，下标为0的项用于记录总数
	for (int i = 0; i < nonFrozenCount; i++)
	{
		// 把没亮出来的手牌分三种花色放好
		if (cards[i][0] == 'W')
		{
			num[0][cards[i][1] - '0']++;
			num[0][0]++;
		}
		if (cards[i][0] == 'B')
		{
			num[1][cards[i][1] - '0']++;
			num[1][0]++;
		}
		if (cards[i][0] == 'T')
		{
			num[2][cards[i][1] - '0']++;
			num[2][0]++;
		}
	}
	return CheckHu(num[0]) && CheckHu(num[1]) && CheckHu(num[2]);
}

//把饼条万变数字
int Getnum(string x) {
	int a;
	switch (x[0])
	{
		case 'B':
			a = 0;
			break;
		case 'b':
			a = 0;
			break;
		case 'T':
			a = 10;
			break;
		case 't':
			a = 10;
			break;
		case 'W':
			a = 20;
			break;
		case 'w':
			a = 20;
			break;
	}
	a += x[1] - 48;
	return a;
}

//估价函数。
double Assess(int num, int Xuyao, int Shengxia)
{
	//剩下的不够，这种胡牌方式不行！
	if (Xuyao > Shengxia) return 0;
	//我不需要，价值不减
	if (Xuyao == 0) return 1;
	double value = 1;
	double x;
	//考虑剩下牌的数量，剩下的少的价值低
	switch (Shengxia)
	{
		case 4:
			x = 1;
			break;
		case 3:
			x = 0.999;
			break;
		case 2:
			x = 0.998;
			break;
		case 1:
			x = 0.995;
			break;
	}
	for (int i = 0; i < Xuyao; ++i)
		value *= x;
	return value;
}

double Loneliness[3][10];

double GetValue(int x, int ShouPai[10], int MuBiao[10], double adds)
{
	int BasicValue = 0;//用来统计我已经有了这种胡牌方式中的多少张。
	double Discount = adds;//将要减少的价值。
	for (int i = 1; i <= 9; ++i)
	{
		if (ShouPai[i] < MuBiao[i]) Discount *= Assess(i, MuBiao[i] - ShouPai[i], paichi[x * 10 + i]);//手牌不够要摸
		BasicValue += min(ShouPai[i], MuBiao[i]);
		if (MuBiao[i] > 0)
			Discount *= Loneliness[x][i];//考虑牌的孤独值（没有相邻则孤独值大）
	}
	return BasicValue * Discount;//返回估价
}

int sortnum [3][10];//用于统计并保存未明示的手牌
int sortnum_2 [3][10];//用于统计并保存未明示的手牌
int sortinnum [3][10];//用于统计并保存明示的手牌

void SortHu ()//计算sortnum
{
	SortAndGetFrozenCount();
	memset(sortnum, 0, sizeof(sortnum)); // 顺序：筒、条、万，下标为0的项用于记录总数
	memset(sortinnum, 0, sizeof(sortinnum));
	for (int i = 0; i < cardCount; i++)
	{
		// 没亮出来的手牌
		if (cards[i][0] == 'B')
		{
			sortnum[0][cards[i][1] - '0']++;
			sortnum[0][0]++;
		}
		if (cards[i][0] == 'T')
		{
			sortnum[1][cards[i][1] - '0']++;
			sortnum[1][0]++;
		}
		if (cards[i][0] == 'W')
		{
			sortnum[2][cards[i][1] - '0']++;
			sortnum[2][0]++;
		}
		//亮出来的手牌
		if (cards[i][0] == 'b')
		{
			sortinnum[0][cards[i][1] - '0']++;
			sortinnum[0][0]++;
		}
		if (cards[i][0] == 't')
		{
			sortinnum[1][cards[i][1] - '0']++;
			sortinnum[1][0]++;
		}
		if (cards[i][0] == 'w')
		{
			sortinnum[2][cards[i][1] - '0']++;
			sortinnum[2][0]++;
		}
	}
}

double maxValue [3]; //每种花色当前分配最优价值
int maxNum [3][10]; //当前的最优目标牌型
int aimnum [3][10]; //当前的目标牌型
int maxnum [3][10]; //最终的最优目标牌型
double maxvalue; //最终最优牌型的估价
struct data {
	int Cnt[3];
	int JuZi[3][5][2];
} Save, tmpSave, BestSave;

void MeiJuJuZi (int no, int last, double adds, int ttmp)//no表示花色 last表示还要组多少个句子（其实应该用rest 【=。=】）
{
	if (last == 0)//句子枚举完毕
	{
		//计算此花色估价
		double x = GetValue (no, sortnum[no], aimnum[no], adds);
		if (x > maxValue[no])
		{
			//更新当前最优牌型
			maxValue[no] = x;
			for (int i = 0; i <= 9; i++) maxNum[no][i] = aimnum[no][i];
			memcpy(tmpSave.JuZi[no], Save.JuZi[no], sizeof(Save.JuZi[no]));
		}
		return;
	}
	//枚举ABC型的句子
	for (int i = ttmp; i <= 7; i++)
	{
		aimnum[no][0] += 3;
		for (int j = i; j <= i + 2; j++) aimnum[no][j] ++;
		Save.JuZi[no][last - 1][0] = 0;
		Save.JuZi[no][last - 1][1] = i;
		MeiJuJuZi (no, last - 1, adds, i);
		aimnum[no][0] -= 3;
		for (int j = i; j <= i + 2; j++) aimnum[no][j] --;
	}
	//枚举AAA型的句子
	for (int i = ttmp; i <= 9; i++)
	//我至少有一张A，或没有找到过一种胡牌牌型
	if (sortnum[no][i] > 0 || maxvalue == -1)
	{
		aimnum[no][0] += 3;
		aimnum[no][i] += 3;
		Save.JuZi[no][last - 1][0] = 1;
		Save.JuZi[no][last - 1][1] = i;
		MeiJuJuZi (no, last - 1, adds * 1.00201, i);
		aimnum[no][0] -= 3;
		aimnum[no][i] -= 3;
	}
}

//分配句子
void FenPeiJuZi (int x)
{
	for (int i = 0; i <= x; i++)
		for (int j = 0; j <= x - i; j++)
		{
			memset(maxNum, 0, sizeof(maxNum));
			memset(maxValue, 0, sizeof(maxValue));
			//分配饼i个句子， 条j个句子， 万 x - i - j个句子
			MeiJuJuZi (0, i, 1, 1);
			MeiJuJuZi (1, j, 1, 1);
			MeiJuJuZi (2, x - i - j, 1, 1);
			//这样分配的价值
			if (maxValue[0] + maxValue[1] + maxValue[2] > maxvalue)
			{
				//更优则更新
				maxvalue = maxValue[0] + maxValue[1] + maxValue[2];
				memcpy(BestSave.JuZi, tmpSave.JuZi, sizeof(tmpSave.JuZi));
				BestSave.Cnt[0] = i;
				BestSave.Cnt[1] = j;
				BestSave.Cnt[2] = x - i - j;
				for (int k = 0; k <= 2; k++)
					for (int l= 0; l <= 9; l++)
						maxnum[k][l] = maxNum[k][l];
			}
		}
}

//估算每一个未明示的手牌的孤独值
void Calc_Loneliness()
{
	for (int i = 0; i < 3; ++i)
		for (int j = 1; j <= 9; ++j)
			Loneliness[i][j] = 1;
	for (int i = 0; i < nonFrozenCount; ++i)
	{
		int x = Getnum(cards[i]);
		int a = x / 10, b = x % 10;
		double p;
		if (sortnum[a][b] > 1) p = 1;
		else if ((b > 1 && sortnum[a][b - 1] > 0) || (b < 9 && sortnum[a][b + 1] > 0)) p = 0.999;
		else if ((b > 2 && sortnum[a][b - 2] > 0) || (b < 8 && sortnum[a][b + 2] > 0)) p = 0.995;
		else p = 0;
		//1、2、8、9更孤独。
		if (b == 1 || b == 9) p *= 0.998;
		else if (b == 2 || b == 8) p *= 0.999;
		Loneliness[a][b] = p;
	}
}

//找到最优的胡牌方法
void FindBest ()
{
	SortHu ();//先顺牌
	Calc_Loneliness();//估算孤独值
	const int adds = 14 - nonFrozenCount;//把明示的牌也算入估价中
	maxvalue = -1;
	memset(maxnum, 0, sizeof(maxnum));
	//枚举将牌
	for (int i = 1; i <= 29; i++)
	//（我至少有一张这个，才能当将牌）
	//除非我一种胡牌牌型都没有找到
	if (i % 10 != 0 && (sortnum[i / 10][i % 10] > 0 || maxvalue == -1)) 
	{
		memset(aimnum, 0, sizeof(aimnum));
		aimnum[i / 10][i % 10] = 2;
		aimnum[i / 10][0] = 2;
		int lastjuzi = nonFrozenCount / 3;//假定句子都为3张
		FenPeiJuZi ( lastjuzi ); //分配句子
	}
	maxvalue += adds;
}

int JiHuiZhi[3];//保存三种花色的机会值
int maxJiHuiZhi[3];
int maxJiHui;
//流局时，计算海明距离，以及胡牌机会值。
//我没胡牌就是悲剧= =||
double GetValue_2 (int x, int ShouPai[10], int MuBiao[10])
{
	double BasicValue = 0;
	JiHuiZhi[x] = 999;
	for (int i = 1; i <= 9; i++)
	{
		BasicValue += abs (ShouPai[i] - MuBiao[i]);
		if (paichi[x * 10 + i] > 0) LiuJu[x][i] = 999;
		if (MuBiao[i] > ShouPai[i]) JiHuiZhi[x] = min(JiHuiZhi[x], LiuJu[x][i]);
	}
	return 100 - BasicValue;
}

//流局时枚举句子
void MeiJuJuZi_2 (int no, int last, int ttmp)//no表示花色 last表示还要组多少个句子
{
	if (last == 0)
	{
		double x = GetValue_2 (no, sortnum[no], aimnum[no]);
		if (x > maxValue[no] || (x == maxValue[no] && JiHuiZhi[no] > maxJiHuiZhi[no]))
		{
			maxJiHuiZhi[no] = JiHuiZhi[no];
			maxValue[no] = x;
			for (int i = 0; i <= 9; i++) maxNum[no][i] = aimnum[no][i];
		}
		return;
	}
	//枚举ABC型
	for (int i = ttmp; i <= 7; i++)
	{
		aimnum[no][0] += 3;
		for (int j = i; j <= i + 2; j++) aimnum[no][j] ++;
		MeiJuJuZi_2 (no, last - 1, i);
		aimnum[no][0] -= 3;
		for (int j = i; j <= i + 2; j++) aimnum[no][j] --;
	}
	//枚举AAA型
	for (int i = ttmp; i <= 9; i++)
	if (sortnum[no][i] > 0 || maxvalue == -1)
	{
		aimnum[no][0] += 3;
		aimnum[no][i] += 3;
		MeiJuJuZi_2 (no, last - 1, i);
		aimnum[no][0] -= 3;
		aimnum[no][i] -= 3;
	}
}

//流局时分配句子
void FenPeiJuZi_2 (int x)
{
	for (int i = 0; i <= x; i++)
		for (int j = 0; j <= x - i; j++)
		{
			memset(maxNum, 0, sizeof(maxNum));
			memset(maxValue, 0, sizeof(maxValue));
			memset(maxJiHuiZhi, 0, sizeof(maxJiHuiZhi));
			//分配筒i个句子， 条j个句子， 万 x - i - j个句子
			MeiJuJuZi_2 (0, i, 1);
			MeiJuJuZi_2 (1, j, 1);
			MeiJuJuZi_2 (2, x - i - j, 1);
			int JiHui = min(min(maxJiHuiZhi[0], maxJiHuiZhi[1]), maxJiHuiZhi[2]);
			//如果这样分配的价值和更优
			if (maxValue[0] + maxValue[1] + maxValue[2] > maxvalue || (maxValue[0] + maxValue[1] + maxValue[2] == maxvalue && JiHui > maxJiHui))
			{
				maxJiHui = JiHui;
				maxvalue = maxValue[0] + maxValue[1] + maxValue[2];
				for (int k = 0; k <= 2; k++)
					for (int l= 0; l <= 9; l++)
						maxnum[k][l] = maxNum[k][l];
			}
		}
}

//流局时找到最优目标牌型
void FindBest_2 ()
{
	SortHu ();//先顺牌
	maxvalue = -1;
	maxJiHui = -1;
	memset(maxnum, 0, sizeof(maxnum));
	for (int i = 1; i <= 29; i++)
	if (i % 10 != 0 && (sortnum[i / 10][i % 10] > 0 || maxvalue == -1)) 
	{
		memset(aimnum, 0, sizeof(aimnum));
		aimnum[i / 10][i % 10] = 2;
		aimnum[i / 10][0] = 2;
		int lastjuzi = nonFrozenCount / 3;//假定句子都为3张
		FenPeiJuZi_2 ( lastjuzi ); //分配句子
	}
}

void TingPai (int Ct, string pai [20] , bool  tingpai [30])
{
	memset(tingpai, 0, sizeof(bool) * 30);
	string tingcard;//听哪张牌
	string fuben [20];
	for (int i = 0; i <= cardCount - 1; i++)
		fuben[i] = cards[i];//转存cards
	int Paifu = cardCount;//转存cardCount
	for (int i = 0; i <= Ct - 1; i++) cards[i] = pai [i];
	cardCount = Ct;//复制
	for (int i = 1; i <= 29; i++) 
	if (i % 10 != 0)
	{
		tingcard = Getstr(i);
		cards[cardCount++] = tingcard;//加入牌
		if (Hu()) tingpai [i] = 1;
		for (int j = 0; j < cardCount; j++)
			if (cards[j] == tingcard)
			{
				//去掉加入的牌
				cards[j] = cards[--cardCount];
				break;
			}
	}
	//恢复cards
	cardCount = Paifu;
	for (int i = 0; i < cardCount; i++) cards[i] = fuben[i];
}

int TingPaiShuMu()
{
	bool tingpai[30];
	string Ck[20];
	for (int i = 0; i < cardCount; i++) Ck[i] = cards[i];
	TingPai(cardCount, Ck, tingpai);
	int tot = 0;
	int cnt = 0;
	for (int i = 0; i < 30; ++i)
		if (tingpai[i] && paichi[i] > 0) tot += paichi[i], ++cnt;
	return cnt / 2 + tot;
}

bool FindLonely(string &p)
{
	return 0;
	const int pt[10] = {0, 1, 9, 2, 3, 4, 5, 6, 7, 8};
	//散牌
	for (int j = 1; j <= 9; ++j)
		for (int a = 0; a < 3; ++a)
		{
			int b = pt[j];
			if (sortnum[a][b] != 1) continue;
			if (b != 1 && sortnum[a][b - 1] > 0) continue;
			if (b != 9 && sortnum[a][b + 1] > 0) continue;
			if (b >= 3 && sortnum[a][b - 2] > 0) continue;
			if (b <= 7 && sortnum[a][b + 2] > 0) continue;
			p += Getstr(a * 10 + b);
			return 1;
		}
	//去除01110/02220/03330/>04440<
	for (int i = 0; i <= 2; ++i)
		for (int j = 2; j <= 8; ++j)
			if (sortnum[i][j] > 0 &&
			    sortnum[i][j - 1] == sortnum[i][j] &&
			    sortnum[i][j + 1] == sortnum[i][j] &&
			   ((j <= 2 || sortnum[i][j - 2] == 0) ||
			    (j >= 8 || sortnum[i][j + 2] == 0))
				)
				sortnum[i][j - 1] = sortnum[i][j] = sortnum[i][j + 1] = 0;
	//特殊处理  ###
	for (int i = 0; i <= 2; ++i)
		if (sortnum[i][9] == 1 &&
		    sortnum[i][8] == 1 &&
		    sortnum[i][7] == 0 &&
		    sortnum[i][6] == 1 &&
		    sortnum[i][5] == 0 &&
		    sortnum[i][4] == 0)
		{
			p += Getstr(i * 10 + 9);
			return 1;
		}
	for (int i = 0; i <= 2; ++i)
		if (sortnum[i][1] == 1 &&
		    sortnum[i][2] == 1 &&
		    sortnum[i][3] == 0 &&
		    sortnum[i][4] == 1 &&
		    sortnum[i][5] == 0 &&
		    sortnum[i][6] == 0)
		{
			p += Getstr(i * 10 + 1);
			return 1;
		}
	//0012100
	for (int i = 0; i <= 2; ++i)
		for (int j = 2; j <= 8; ++j)
			if ((j < 4 || sortnum[i][j - 3] == 0) &&
			    (j < 3 || sortnum[i][j - 2] == 0) &&
			    sortnum[i][j - 1] == 1 &&
			    sortnum[i][j    ] == 2 &&
			    sortnum[i][j + 1] == 1 &&
			    (j > 7 || sortnum[i][j + 2] == 0) &&
			    (j > 6 || sortnum[i][j + 3] == 0))
			{
				p += Getstr(i * 10 + j);
				return 1;
			}
	//0010110
	for (int i = 0; i <= 2; ++i)
		for (int j = 7; j <= 9; ++j)
			if (sortnum[i][j - 6] == 0 &&
			    sortnum[i][j - 5] == 0 &&
			    sortnum[i][j - 4] == 1 &&
			    sortnum[i][j - 3] == 0 &&
			    sortnum[i][j - 2] == 1 &&
			    sortnum[i][j - 1] == 1 &&
			    sortnum[i][j    ] == 0)
			{
				p += Getstr(i * 10 + j - 4);
				return 1;
			}
	for (int i = 0; i <= 2; ++i)
		if (sortnum[i][9] == 0 &&
		    sortnum[i][8] == 1 &&
		    sortnum[i][7] == 1 &&
		    sortnum[i][6] == 0 &&
		    sortnum[i][5] == 1 &&
		    sortnum[i][4] == 0 &&
		    sortnum[i][3] == 0)
		{
			p += Getstr(i * 10 + 5);
			return 1;
		}
	for (int i = 0; i <= 2; ++i)
		if (sortnum[i][1] == 0 &&
		    sortnum[i][2] == 1 &&
		    sortnum[i][3] == 1 &&
		    sortnum[i][4] == 0 &&
		    sortnum[i][5] == 1 &&
		    sortnum[i][6] == 0 &&
		    sortnum[i][7] == 0)
		{
			p += Getstr(i * 10 + 5);
			return 1;
		}
	for (int i = 0; i <= 2; ++i)
		for (int j = 7; j <= 9; ++j)
			if (sortnum[i][j - 6] == 0 &&
			    sortnum[i][j - 5] == 1 &&
			    sortnum[i][j - 4] == 1 &&
			    sortnum[i][j - 3] == 0 &&
			    sortnum[i][j - 2] == 1 &&
			    sortnum[i][j - 1] == 0 &&
			    sortnum[i][j    ] == 0)
			{
				p += Getstr(i * 10 + j - 2);
				return 1;
			}
	for (int i = 0; i <= 2; ++i)
		if (sortnum[i][9] == 1 &&
		    sortnum[i][8] >= 1 &&
		    sortnum[i][7] == 0 &&
		    sortnum[i][6] == 1 &&
		    sortnum[i][5] == 0 &&
		    sortnum[i][4] >= 1 &&
		    sortnum[i][3] >= 1
		    )
		{
			p += Getstr(i * 10 + 9);
			return 1;
		}
	for (int i = 0; i <= 2; ++i)
		if (sortnum[i][1] == 1 &&
		    sortnum[i][2] >= 1 &&
		    sortnum[i][3] == 0 &&
		    sortnum[i][4] == 1 &&
		    sortnum[i][5] == 0 &&
		    sortnum[i][6] >= 1 &&
		    sortnum[i][7] >= 1
		    )
		{
			p += Getstr(i * 10 + 1);
			return 1;
		}
	const int celue[12][10] = {
				{1, 1, 2, 0, 1, 0, 0, 0, 0, 0},
				{5, 0, 1, 2, 0, 1, 0, 0, 0, 0},
				{6, 0, 0, 1, 2, 0, 1, 0, 0, 0},
				{7, 0, 0, 0, 1, 2, 0, 1, 0, 0},
				{8, 0, 0, 0, 0, 1, 2, 0, 1, 0},
				{9, 0, 0, 0, 0, 0, 1, 2, 0, 1},
				{1, 1, 0, 2, 1, 0, 0, 0, 0, 0},
				{2, 0, 1, 0, 2, 1, 0, 0, 0, 0},
				{3, 0, 0, 1, 0, 2, 1, 0, 0, 0},
				{4, 0, 0, 0, 1, 0, 2, 1, 0, 0},
				{5, 0, 0, 0, 0, 1, 0, 2, 1, 0},
				{9, 0, 0, 0, 0, 0, 1, 0, 2, 1}
	};
	for (int k = 0; k < 12; ++k)
		for (int i = 0; i < 3; ++i)
		{
			bool flag = 1;
			for (int j = 1; j <= 9; ++j)
			if (celue[k][j] != sortnum[i][j])
			{
				flag = 0;
				break;
			}
			if (!flag) continue;
			p += Getstr(i * 10 + celue[k][0]);
			return 1;
		}
	//01210
	for (int i = 0; i <= 2; ++i)
		for (int j = 2; j <= 8; ++j)
			if ((j < 3 || sortnum[i][j - 2] == 0) &&
			    sortnum[i][j - 1] == 1 &&
			    sortnum[i][j    ] == 2 &&
			    sortnum[i][j + 1] == 1 &&
			    (j > 7 || sortnum[i][j + 2] == 0))
			{
				p += Getstr(i * 10 + j);
				return 1;
			}
	//去除01110/02220/03330/>04440<
	for (int i = 0; i <= 2; ++i)
		for (int j = 2; j <= 8; ++j)
			if ((j <= 2 || sortnum[i][j - 2] == 0) &&
			    (j >= 8 || sortnum[i][j + 2] == 0)
				)
			{
				int x = min(min(sortnum[i][j - 1], sortnum[i][j]), sortnum[i][j + 1]);
				sortnum[i][j - 1] -= x;
				sortnum[i][j] -= x;
				sortnum[i][j + 1] -= x;
			}
	//散牌
	for (int a = 0; a < 3; ++a)
		for (int j = 1; j <= 9; ++j)
		{
			int b = pt[j];
			if (sortnum[a][b] != 1) continue;
			if (b != 1 && sortnum[a][b - 1] > 0) continue;
			if (b != 9 && sortnum[a][b + 1] > 0) continue;
			if (b >= 3 && sortnum[a][b - 2] > 0) continue;
			if (b <= 7 && sortnum[a][b + 2] > 0) continue;
			p += Getstr(a * 10 + b);
			return 1;
		}
	return 0;
}

void CalcSortnum()
{
	memcpy(sortnum_2, sortnum, sizeof(sortnum));
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < BestSave.Cnt[i]; ++j)
			if (BestSave.JuZi[i][j][0] == 0)
			{
				int t = BestSave.JuZi[i][j][1];
				if (sortnum_2[i][t] > 0 && sortnum_2[i][t + 1] > 0 && sortnum_2[i][t + 2] > 0)
				{
					--sortnum_2[i][t];
					--sortnum_2[i][t + 1];
					--sortnum_2[i][t + 2];
				}
			}
	
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < BestSave.Cnt[i]; ++j)
			if (BestSave.JuZi[i][j][0] == 1)
			{
				int t = BestSave.JuZi[i][j][1];
				if (sortnum_2[i][t] >= 3)
					sortnum_2[i][t] -= 3;
			}
}

int main()
{
	srand(time(0));

	// 读入JSON
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);

	int turnID = input["responses"].size();

	// 分析自己收到的输入和自己过往的输出，并恢复状态
	for (int i = 0; i < turnID; i++)
	{
		istringstream in(input["requests"][i].asString()),
			out(input["responses"][i].asString());
		string act;

		int type, who;
		string card, what;

		// 获得信息类型
		in >> type;
		switch (type)
		{
		case 0:
			// 告知编号
			in >> myID;
			for (int k = 0; k < 3; ++k)
				for (int j = 1; j <= 9; ++j)
					LiuJu[k][j] = 999;
			cnt = 0;
			// response一定是PASS，不用看了
			break;
		case 1:
			// 告知手牌
			// 初始化牌池
			for (int j = 0; j < 30; j++)
				paichi[j] = 4;
			for (int j = 0; j < 13; j++)
			{
				in >> cards[j];
				--paichi[Getnum(cards[j])];
			}
			cardCount = 13;
			// response一定是PASS，不用看了
			break;
		case 2:
			// 摸牌，收入手牌
			in >> cards[cardCount++];
			paichi[Getnum(cards[cardCount - 1])] -= 1;

			// 然后我做了act
			out >> act;
			if (act == "PLAY")
			{
				// 当时我打出了……
				out >> act;
				//统计机会值
				int nx = Getnum(act);
				lastcnt = LiuJu[nx / 10][nx % 10];
				lastnx = nx;
				LiuJu[nx / 10][nx % 10] = ++cnt;
				// ……一张act！
				for (int j = 0; j < cardCount; j++)
				{
					if (cards[j] == act)
					{
						// 去掉这张牌，拿最后一张牌填这个空位
						cards[j] = cards[--cardCount];
						break;
					}
				}
			}
			else if (act == "GANG")
			{
				// 当时我杠了……
				out >> act;
				//统计机会值
				int nx = Getnum(act);
				lastcnt = LiuJu[nx / 10][nx % 10];
				lastnx = nx;
				LiuJu[nx / 10][nx % 10] = ++cnt;
				// 一张act！（act是大写的）
				// 在手牌里把这个牌变为小写（明示）
				for (int j = 0; j < cardCount; j++)
				{
					if (cards[j] == act)
						cards[j][0] += 'a' - 'A'; // 变成小写
				}
			}
			// HU不可能出现
			break;
		case 3:
			// 别人的动作
			in >> who >> what >> card;
			
			// 不是打牌的话，response一定是PASS，不用看了
			if (what != "PLAY")
			{
				//打出来的牌被收回别人的手牌，恢复机会值
				if (card != "SELF")
				{
					int xx = Getnum(card);
					if (xx == lastnx)
						LiuJu[xx / 10][xx % 10] = lastcnt;
				}
				//统计牌池
				if (who != myID)
				{
					if (what == "GANG") paichi[Getnum(card)] = 0;
					else if (what == "PENG") paichi[Getnum(card)] -= 2;
				}
				break;
			}
			else {
				//统计机会值
				int nx = Getnum(card);
				lastcnt = LiuJu[nx / 10][nx % 10];
				lastnx = nx;
				LiuJu[nx / 10][nx % 10] = ++cnt;
				if (who != myID) 
					paichi[Getnum(card)] -= 1;
			}

			// 然后我又做了act
			out >> act;

			if (act == "PENG")
			{
				// 先看看这一回合有没有人胡
				{
					int tmp;
					string act, card2;
					istringstream nextInput(input["requests"][i + 1].asString());
					if (nextInput >> tmp >> tmp >> act >> card2 && act == "HU" && card == card2)
						break;
				}

				//打出来的牌被我收回手牌，恢复机会值
				LiuJu[lastnx / 10][lastnx % 10] = lastcnt;
				// 当时我碰牌了
				// 在手牌里把两张card变为小写（明示）
				int count = 0;
				for (int j = 0; j < cardCount; j++)
				{
					if (cards[j] == card) {
						cards[j][0] += 'a' - 'A'; // 变成小写
						if (++count == 2)
							break;
					}
				}
				// 再把card收入手牌
				card[0] += 'a' - 'A';
				cards[cardCount++] = card;

				// 然后我出了……
				out >> act;
				//统计机会值
				int nx = Getnum(act);
				lastcnt = LiuJu[nx / 10][nx % 10];
				lastnx = nx;
				LiuJu[nx / 10][nx % 10] = ++cnt;
				// ……一张act！
				for (int j = 0; j < cardCount; j++)
				{
					if (cards[j] == act)
					{
						// 去掉这张牌，拿最后一张牌填这个空位
						cards[j] = cards[--cardCount];
						break;
					}
				}
			}
			else if (act == "GANG")
			{
				
				// 先看看这一回合有没有人胡
				{
					int tmp;
					string act, card;
					istringstream nextInput(input["requests"][i + 1].asString());
					if (nextInput >> tmp >> tmp >> act >> card && act == "HU" && card != "SELF")
						break;
				}

				//打出来的牌被我收回手牌，恢复机会值
				LiuJu[lastnx / 10][lastnx % 10] = lastcnt;
				// 当时我杠牌了
				// 在手牌里把card都变为小写（明示）
				for (int j = 0; j < cardCount; j++)
				{
					if (cards[j] == card)
						cards[j][0] += 'a' - 'A'; // 变成小写
				}
				// 再把card收入手牌
				card[0] += 'a' - 'A';
				cards[cardCount++] = card;
			}
			// HU不可能出现，PASS不用处理
			break;
		case 4:
			// 这种情况不可能出现
			;
		}
	}

	// 看看自己本回合输入
	istringstream in(input["requests"][turnID].asString());
	int type, who;
	string act, card, temp, myAction = "PASS";
	in >> type;
	if (type == 2)
	{
		// 轮到我摸牌
		in >> card;
		--paichi[Getnum(card)];
		
		//加入手牌
		cards[cardCount++] = card;

		// 能不能胡？
		if (Hu()) // 注意此时牌已经排序
			myAction = "HU";
		else
		{
			SortHu();
			//备份手牌
			cardCount_2 = cardCount;
			for (int i = 0; i < cardCount; ++i)
				cards_2[i] = cards[i];
			//计算不杠的估价
			FindBest ();
			double oldvalue = maxvalue;
			myAction = "PLAY ";
			if (!FindLonely(myAction))
			{	
				cardCount = cardCount_2;
				for (int i = 0; i < cardCount; ++i)
					cards[i] = cards_2[i];
				SortHu();
				CalcSortnum();
				int minBaoTuan = 10000;
				for (int i = 0; i < 3; ++i)
					for (int j = 1; j <= 9; ++j)
					{
						if (sortnum[i][j] > maxnum[i][j])
						{
							int BaoTuan = 0;
							BaoTuan += sortnum_2[i][j] * 4;
							if (j >= 2) BaoTuan += sortnum_2[i][j - 1] * 2;
							if (j <= 8) BaoTuan += sortnum_2[i][j + 1] * 2;
							if (j >= 3) BaoTuan += sortnum_2[i][j - 2];
							if (j <= 7) BaoTuan += sortnum_2[i][j + 2];
							if (j == 9 || j == 1) BaoTuan -= 2;
							if (j == 8 || j == 2) BaoTuan -= 1;
							if (BaoTuan < minBaoTuan)
							{
								myAction = "PLAY " + Getstr(i * 10 + j);
								minBaoTuan = BaoTuan;
							}
						}
				}
			}
			int maxTing = 0; int cardleft = 100;
			for (int i = 0; i < cardCount_2; ++i)
			if (cards_2[i][0] <= 'Z')
			{
				cardCount = cardCount_2;
				for (int j = 0; j < cardCount; ++j)
					cards[j] = cards_2[j];
				for (int j = 0; j < cardCount; j++)
				{
					if (cards[j] == cards_2[i])
					{
						// 去掉这张牌，拿最后一张牌填这个空位
						cards[j] = cards[--cardCount];
						break;
					}
				}
				int Ting = TingPaiShuMu();
				if (Ting == 0) continue;
				int tmp = Getnum(cards_2[i]);
				if (Ting > maxTing || (Ting == maxTing && paichi[tmp] + sortnum[tmp / 10][tmp % 10] < cardleft))
				{
					maxTing = Ting;
					cardleft = paichi[tmp] + sortnum[tmp / 10][tmp % 10];
					myAction = "PLAY " + cards_2[i];
				}
			}
			double GangValue = -1;
			string GangCard;
			for (int a = 0; a < 3; ++a)
			for (int b = 1; b <= 9; ++b)
			if (sortnum[a][b] + sortinnum[a][b] == 4 && sortnum[a][b] > 0)
			{
				//恢复手牌
				card = Getstr(a * 10 + b);
				cardCount = cardCount_2;
				for (int i = 0; i < cardCount; ++i)
				{
					cards[i] = cards_2[i];
					if (cards[i][0] == card[0] && cards[i][1] == card[1]) cards[i][0] += 'a' - 'A';
				}
				//计算杠的估价
				FindBest ();
				//看看杠这张是不是更好
				if (maxvalue > GangValue)
				{
					GangValue = maxvalue;
					GangCard = card;
				}
			}
			//到底杠不杠？
			if (GangValue > oldvalue * 0.99)
				myAction = "GANG " + GangCard;
		}	
	}
	else if (type == 3)
	{
		// 其他玩家……
		in >> who >> act >> card;

		if (act == "PLAY") // 除非别人打牌，否则什么也干不了
		{
			// 先收进来

			// 提示：
			// 如果只能PASS，
			// 手牌也不用恢复，
			// 因为下次会重新计算
			--paichi[Getnum(card)];
			for (int i = 0; i < cardCount; i++) cards_2[i] = cards[i];
			cardCount_2 = cardCount;
			cards[cardCount++] = card;
			// 能不能胡？
			if (Hu()) // 注意此时牌已经排序
				myAction = "HU";
			else
			{
			// 能不能杠/碰？
				int count = 0;
				string temp = card;
				temp[0] += 'a' - 'A';
				int count_2 = 0;
				for (int i = 0; i < cardCount; i++)
				{
					if (cards[i] == temp)
						count_2++;
					else if (cards[i] == card)
						count++;
				}
				myAction = "PASS";
				maxvalue = -1;
				//看看要不要碰、杠
				if (count + count_2 >= 3) 
				{
					double oldvalue = -1, PengValue = -1, GangValue = -1;
					//恢复原有手牌（不包括别人打的那张）
					for (int i = 0; i < cardCount_2; i++) cards[i] = cards_2[i];
					cardCount = cardCount_2;
					//计算不碰、不杠的估价
					FindBest ();
					oldvalue = maxvalue;
					bool flag = 1;
					//如果听的牌多就不碰了
					
					int BuPengTing;
					if (count >= 3)
					{
						bool tingpai[30];
						string Ck[20];
						for (int i = 0; i < cardCount_2; i++) Ck[i] = cards_2[i];
						TingPai(cardCount_2, Ck, tingpai);
						int tot = 0, cnt = 0;
						for (int i = 0; i < 30; ++i)
							if (tingpai[i] && paichi[i] > 0) tot += paichi[i], ++cnt;
						BuPengTing = tot + cnt / 2;
						if (tot >= 6) flag = 0;
					}
					//碰：
					if (count >= 3 && flag)
					{
						//恢复原有手牌（不包括别人打的那张）
						for (int i = 0; i < cardCount_2; i++) cards[i] = cards_2[i];
						cardCount = cardCount_2;
						//把别人打的那张牌加入手牌
						cards[cardCount++] = card;
						int PengCount = 0;
						for (int i = 0; i < cardCount; i++)
						{
							if (cards[i] == card) cards[i] = temp, PengCount++;
							if (PengCount == 3) break;
						}

						//计算碰的估价
						FindBest();
						PengValue = maxvalue;
						if (PengValue > oldvalue * 0.99)
						{
							myAction = "PENG ";
							if (!FindLonely(myAction))
							{	
								cardCount = cardCount_2;
								for (int i = 0; i < cardCount; ++i)
									cards[i] = cards_2[i];
								cards[cardCount++] = card;
								SortHu();
								CalcSortnum();
								int minBaoTuan = 10000;
								for (int i = 0; i < 3; ++i)
									for (int j = 1; j <= 9; ++j)
									{
										if (sortnum[i][j] > maxnum[i][j])
										{
											int BaoTuan = 0;
											BaoTuan += sortnum_2[i][j] * 4;
											if (j >= 2) BaoTuan += sortnum_2[i][j - 1] * 2;
											if (j <= 8) BaoTuan += sortnum_2[i][j + 1] * 2;
											if (j >= 3) BaoTuan += sortnum_2[i][j - 2];
											if (j <= 7) BaoTuan += sortnum_2[i][j + 2];
											if (j == 9 || j == 1) BaoTuan -= 2;
											if (j == 8 || j == 2) BaoTuan -= 1;
											if (BaoTuan < minBaoTuan)
											{
												myAction = "PENG " + Getstr(i * 10 + j);
												minBaoTuan = BaoTuan;
											}
										}
								}
							}
							oldvalue = PengValue;
						}
						int maxTing = BuPengTing + 1; int cardleft = 100;
						for (int i = 0; i < cardCount_2; ++i)
						if (cards_2[i][0] <= 'Z')
						{
							cardCount = cardCount_2;
							for (int j = 0; j < cardCount; ++j)
								cards[j] = cards_2[j];							
							//把别人打的那张牌加入手牌
							cards[cardCount++] = card;
							int PengCount = 0;
							for (int j = 0; j < cardCount; j++)
							{
								if (cards[j] == card) cards[j] = temp, PengCount++;
								if (PengCount == 3) break;
							}							
							for (int j = 0; j < cardCount; j++)
							{
								if (cards[j] == cards_2[i])
								{
									// 去掉这张牌，拿最后一张牌填这个空位
									cards[j] = cards[--cardCount];
									break;
								}
							}
							SortHu();
							int Ting = TingPaiShuMu();
							if (Ting == 0) continue;
							int tmp = Getnum(cards_2[i]);
							if (Ting > maxTing || (Ting == maxTing && paichi[tmp] + sortnum[tmp / 10][tmp % 10] < cardleft))
							{
								maxTing = Ting;
								cardleft = paichi[tmp] + sortnum[tmp / 10][tmp % 10];
								myAction = "PENG " + cards_2[i];
							}
						}
					}
					//杠：
					if (count >= 4)
					{
						//恢复原有手牌（不包括别人打的那张）
						for (int i = 0; i < cardCount_2; i++) cards[i] = cards_2[i];
						cardCount = cardCount_2;
						//把别人打的那张牌加入手牌
						cards[cardCount++] = card;
						for (int i = 0; i < cardCount_2; i++)
							if (cards[i] == card) cards[i] = temp;
						FindBest ();
						GangValue = maxvalue;
						if (GangValue > oldvalue * 0.99)
							myAction = "GANG";
					}
				}
			}
		}
	}
	else if (type == 4)
	{
		// ……流局了TAT
		// 找一个目标牌型
		myAction = "";
		for (int i = 0; i < cardCount; ++i)
			if (cards[i][0] >= 'a')
			{
				string cat = cards[i];
				myAction += cat + " ";
			}
		FindBest_2 ();
		for (int i = 0; i < 3; i++)
			for (int j = 1; j <= 9; j++)
				for (int k = 1; k <= maxnum[i][j]; k++)
					myAction += Getstr (i * 10 + j) + " ";
		myAction.erase(myAction.length() - 1, 1);
	}
	// 别的情况我只能PASS

	// 输出决策JSON
	Json::Value ret;
	ret["response"] = myAction;
	ret["data"] = "While (1) ez_RP++";
	Json::FastWriter writer;
	cout << writer.write(ret) << endl;
	return 0;
}
