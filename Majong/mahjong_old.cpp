// Mahjong from 2z
// ̰��+����+����
// ���ߣ�GDEZOI
// ��Ϸ��Ϣ��http://www.botzone.org/games#Mahjong

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

int paichi[30]; //δ���ֵ���ʣ�������
/*
 * [01]1B
 * [11]1T
 * [21]1W
 * ��ʾʣ������
 */

char chr (int i) // �� a ת���ַ� aΪ0~9������
{
	char x = i + 48;
	return x;
}

//�����ֱ�ɱ�����
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

// �������򣬻�ÿɴ���Ƶ�����
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

// �ݹ����ܷ��
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
		// ʣ�����Ž���
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
				// ABC�;���
				num[i]--, num[i + 1]--, num[i + 2]--;
				num[0] = num[0] - 3;
				if (CheckHu(num))
					return true;
				num[0] = num[0] + 3;
				num[i]++, num[i + 1]++, num[i + 2]++;
			}
			if (num[i] >= 3)
			{
				// AAA�;���
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

// �ж��ܲ��ܺ�
bool Hu()
{
	SortAndGetFrozenCount();
	if (cardCount < 14 || cardCount > 18)
		return false;
	if ((nonFrozenCount - 2) % 3 != 0)
		return false;
	int num[3][10] = { 0 }; // ˳����Ͳ�������±�Ϊ0�������ڼ�¼����
	for (int i = 0; i < nonFrozenCount; i++)
	{
		// ��û�����������Ʒ����ֻ�ɫ�ź�
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

//�ѱ����������
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

//���ۺ�����
double Assess(int num, int Xuyao, int Shengxia)
{
	//ʣ�µĲ��������ֺ��Ʒ�ʽ���У�
	if (Xuyao > Shengxia) return 0;
	//�Ҳ���Ҫ����ֵ����
	if (Xuyao == 0) return 1;
	double value = 1;
	double x;
	//����ʣ���Ƶ�������ʣ�µ��ٵļ�ֵ��
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
	int BasicValue = 0;//����ͳ�����Ѿ��������ֺ��Ʒ�ʽ�еĶ����š�
	double Discount = adds;//��Ҫ���ٵļ�ֵ��
	for (int i = 1; i <= 9; ++i)
	{
		if (ShouPai[i] < MuBiao[i]) Discount *= Assess(i, MuBiao[i] - ShouPai[i], paichi[x * 10 + i]);//���Ʋ���Ҫ��
		BasicValue += min(ShouPai[i], MuBiao[i]);
		if (MuBiao[i] > 0)
			Discount *= Loneliness[x][i];//�����ƵĹ¶�ֵ��û��������¶�ֵ��
	}
	return BasicValue * Discount;//���ع���
}

int sortnum [3][10];//����ͳ�Ʋ�����δ��ʾ������
int sortnum_2 [3][10];//����ͳ�Ʋ�����δ��ʾ������
int sortinnum [3][10];//����ͳ�Ʋ�������ʾ������

void SortHu ()//����sortnum
{
	SortAndGetFrozenCount();
	memset(sortnum, 0, sizeof(sortnum)); // ˳��Ͳ���������±�Ϊ0�������ڼ�¼����
	memset(sortinnum, 0, sizeof(sortinnum));
	for (int i = 0; i < cardCount; i++)
	{
		// û������������
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
		//������������
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

double maxValue [3]; //ÿ�ֻ�ɫ��ǰ�������ż�ֵ
int maxNum [3][10]; //��ǰ������Ŀ������
int aimnum [3][10]; //��ǰ��Ŀ������
int maxnum [3][10]; //���յ�����Ŀ������
double maxvalue; //�����������͵Ĺ���
struct data {
	int Cnt[3];
	int JuZi[3][5][2];
} Save, tmpSave, BestSave;

void MeiJuJuZi (int no, int last, double adds, int ttmp)//no��ʾ��ɫ last��ʾ��Ҫ����ٸ����ӣ���ʵӦ����rest ��=��=����
{
	if (last == 0)//����ö�����
	{
		//����˻�ɫ����
		double x = GetValue (no, sortnum[no], aimnum[no], adds);
		if (x > maxValue[no])
		{
			//���µ�ǰ��������
			maxValue[no] = x;
			for (int i = 0; i <= 9; i++) maxNum[no][i] = aimnum[no][i];
			memcpy(tmpSave.JuZi[no], Save.JuZi[no], sizeof(Save.JuZi[no]));
		}
		return;
	}
	//ö��ABC�͵ľ���
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
	//ö��AAA�͵ľ���
	for (int i = ttmp; i <= 9; i++)
	//��������һ��A����û���ҵ���һ�ֺ�������
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

//�������
void FenPeiJuZi (int x)
{
	for (int i = 0; i <= x; i++)
		for (int j = 0; j <= x - i; j++)
		{
			memset(maxNum, 0, sizeof(maxNum));
			memset(maxValue, 0, sizeof(maxValue));
			//�����i�����ӣ� ��j�����ӣ� �� x - i - j������
			MeiJuJuZi (0, i, 1, 1);
			MeiJuJuZi (1, j, 1, 1);
			MeiJuJuZi (2, x - i - j, 1, 1);
			//��������ļ�ֵ
			if (maxValue[0] + maxValue[1] + maxValue[2] > maxvalue)
			{
				//���������
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

//����ÿһ��δ��ʾ�����ƵĹ¶�ֵ
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
		//1��2��8��9���¶���
		if (b == 1 || b == 9) p *= 0.998;
		else if (b == 2 || b == 8) p *= 0.999;
		Loneliness[a][b] = p;
	}
}

//�ҵ����ŵĺ��Ʒ���
void FindBest ()
{
	SortHu ();//��˳��
	Calc_Loneliness();//����¶�ֵ
	const int adds = 14 - nonFrozenCount;//����ʾ����Ҳ���������
	maxvalue = -1;
	memset(maxnum, 0, sizeof(maxnum));
	//ö�ٽ���
	for (int i = 1; i <= 29; i++)
	//����������һ����������ܵ����ƣ�
	//������һ�ֺ������Ͷ�û���ҵ�
	if (i % 10 != 0 && (sortnum[i / 10][i % 10] > 0 || maxvalue == -1)) 
	{
		memset(aimnum, 0, sizeof(aimnum));
		aimnum[i / 10][i % 10] = 2;
		aimnum[i / 10][0] = 2;
		int lastjuzi = nonFrozenCount / 3;//�ٶ����Ӷ�Ϊ3��
		FenPeiJuZi ( lastjuzi ); //�������
	}
	maxvalue += adds;
}

int JiHuiZhi[3];//�������ֻ�ɫ�Ļ���ֵ
int maxJiHuiZhi[3];
int maxJiHui;
//����ʱ�����㺣�����룬�Լ����ƻ���ֵ��
//��û���ƾ��Ǳ���= =||
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

//����ʱö�پ���
void MeiJuJuZi_2 (int no, int last, int ttmp)//no��ʾ��ɫ last��ʾ��Ҫ����ٸ�����
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
	//ö��ABC��
	for (int i = ttmp; i <= 7; i++)
	{
		aimnum[no][0] += 3;
		for (int j = i; j <= i + 2; j++) aimnum[no][j] ++;
		MeiJuJuZi_2 (no, last - 1, i);
		aimnum[no][0] -= 3;
		for (int j = i; j <= i + 2; j++) aimnum[no][j] --;
	}
	//ö��AAA��
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

//����ʱ�������
void FenPeiJuZi_2 (int x)
{
	for (int i = 0; i <= x; i++)
		for (int j = 0; j <= x - i; j++)
		{
			memset(maxNum, 0, sizeof(maxNum));
			memset(maxValue, 0, sizeof(maxValue));
			memset(maxJiHuiZhi, 0, sizeof(maxJiHuiZhi));
			//����Ͳi�����ӣ� ��j�����ӣ� �� x - i - j������
			MeiJuJuZi_2 (0, i, 1);
			MeiJuJuZi_2 (1, j, 1);
			MeiJuJuZi_2 (2, x - i - j, 1);
			int JiHui = min(min(maxJiHuiZhi[0], maxJiHuiZhi[1]), maxJiHuiZhi[2]);
			//�����������ļ�ֵ�͸���
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

//����ʱ�ҵ�����Ŀ������
void FindBest_2 ()
{
	SortHu ();//��˳��
	maxvalue = -1;
	maxJiHui = -1;
	memset(maxnum, 0, sizeof(maxnum));
	for (int i = 1; i <= 29; i++)
	if (i % 10 != 0 && (sortnum[i / 10][i % 10] > 0 || maxvalue == -1)) 
	{
		memset(aimnum, 0, sizeof(aimnum));
		aimnum[i / 10][i % 10] = 2;
		aimnum[i / 10][0] = 2;
		int lastjuzi = nonFrozenCount / 3;//�ٶ����Ӷ�Ϊ3��
		FenPeiJuZi_2 ( lastjuzi ); //�������
	}
}

void TingPai (int Ct, string pai [20] , bool  tingpai [30])
{
	memset(tingpai, 0, sizeof(bool) * 30);
	string tingcard;//��������
	string fuben [20];
	for (int i = 0; i <= cardCount - 1; i++)
		fuben[i] = cards[i];//ת��cards
	int Paifu = cardCount;//ת��cardCount
	for (int i = 0; i <= Ct - 1; i++) cards[i] = pai [i];
	cardCount = Ct;//����
	for (int i = 1; i <= 29; i++) 
	if (i % 10 != 0)
	{
		tingcard = Getstr(i);
		cards[cardCount++] = tingcard;//������
		if (Hu()) tingpai [i] = 1;
		for (int j = 0; j < cardCount; j++)
			if (cards[j] == tingcard)
			{
				//ȥ���������
				cards[j] = cards[--cardCount];
				break;
			}
	}
	//�ָ�cards
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
	//ɢ��
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
	//ȥ��01110/02220/03330/>04440<
	for (int i = 0; i <= 2; ++i)
		for (int j = 2; j <= 8; ++j)
			if (sortnum[i][j] > 0 &&
			    sortnum[i][j - 1] == sortnum[i][j] &&
			    sortnum[i][j + 1] == sortnum[i][j] &&
			   ((j <= 2 || sortnum[i][j - 2] == 0) ||
			    (j >= 8 || sortnum[i][j + 2] == 0))
				)
				sortnum[i][j - 1] = sortnum[i][j] = sortnum[i][j + 1] = 0;
	//���⴦��  ###
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
	//ȥ��01110/02220/03330/>04440<
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
	//ɢ��
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

	// ����JSON
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);

	int turnID = input["responses"].size();

	// �����Լ��յ���������Լ���������������ָ�״̬
	for (int i = 0; i < turnID; i++)
	{
		istringstream in(input["requests"][i].asString()),
			out(input["responses"][i].asString());
		string act;

		int type, who;
		string card, what;

		// �����Ϣ����
		in >> type;
		switch (type)
		{
		case 0:
			// ��֪���
			in >> myID;
			for (int k = 0; k < 3; ++k)
				for (int j = 1; j <= 9; ++j)
					LiuJu[k][j] = 999;
			cnt = 0;
			// responseһ����PASS�����ÿ���
			break;
		case 1:
			// ��֪����
			// ��ʼ���Ƴ�
			for (int j = 0; j < 30; j++)
				paichi[j] = 4;
			for (int j = 0; j < 13; j++)
			{
				in >> cards[j];
				--paichi[Getnum(cards[j])];
			}
			cardCount = 13;
			// responseһ����PASS�����ÿ���
			break;
		case 2:
			// ���ƣ���������
			in >> cards[cardCount++];
			paichi[Getnum(cards[cardCount - 1])] -= 1;

			// Ȼ��������act
			out >> act;
			if (act == "PLAY")
			{
				// ��ʱ�Ҵ���ˡ���
				out >> act;
				//ͳ�ƻ���ֵ
				int nx = Getnum(act);
				lastcnt = LiuJu[nx / 10][nx % 10];
				lastnx = nx;
				LiuJu[nx / 10][nx % 10] = ++cnt;
				// ����һ��act��
				for (int j = 0; j < cardCount; j++)
				{
					if (cards[j] == act)
					{
						// ȥ�������ƣ������һ�����������λ
						cards[j] = cards[--cardCount];
						break;
					}
				}
			}
			else if (act == "GANG")
			{
				// ��ʱ�Ҹ��ˡ���
				out >> act;
				//ͳ�ƻ���ֵ
				int nx = Getnum(act);
				lastcnt = LiuJu[nx / 10][nx % 10];
				lastnx = nx;
				LiuJu[nx / 10][nx % 10] = ++cnt;
				// һ��act����act�Ǵ�д�ģ�
				// �������������Ʊ�ΪСд����ʾ��
				for (int j = 0; j < cardCount; j++)
				{
					if (cards[j] == act)
						cards[j][0] += 'a' - 'A'; // ���Сд
				}
			}
			// HU�����ܳ���
			break;
		case 3:
			// ���˵Ķ���
			in >> who >> what >> card;
			
			// ���Ǵ��ƵĻ���responseһ����PASS�����ÿ���
			if (what != "PLAY")
			{
				//��������Ʊ��ջر��˵����ƣ��ָ�����ֵ
				if (card != "SELF")
				{
					int xx = Getnum(card);
					if (xx == lastnx)
						LiuJu[xx / 10][xx % 10] = lastcnt;
				}
				//ͳ���Ƴ�
				if (who != myID)
				{
					if (what == "GANG") paichi[Getnum(card)] = 0;
					else if (what == "PENG") paichi[Getnum(card)] -= 2;
				}
				break;
			}
			else {
				//ͳ�ƻ���ֵ
				int nx = Getnum(card);
				lastcnt = LiuJu[nx / 10][nx % 10];
				lastnx = nx;
				LiuJu[nx / 10][nx % 10] = ++cnt;
				if (who != myID) 
					paichi[Getnum(card)] -= 1;
			}

			// Ȼ����������act
			out >> act;

			if (act == "PENG")
			{
				// �ȿ�����һ�غ���û���˺�
				{
					int tmp;
					string act, card2;
					istringstream nextInput(input["requests"][i + 1].asString());
					if (nextInput >> tmp >> tmp >> act >> card2 && act == "HU" && card == card2)
						break;
				}

				//��������Ʊ����ջ����ƣ��ָ�����ֵ
				LiuJu[lastnx / 10][lastnx % 10] = lastcnt;
				// ��ʱ��������
				// �������������card��ΪСд����ʾ��
				int count = 0;
				for (int j = 0; j < cardCount; j++)
				{
					if (cards[j] == card) {
						cards[j][0] += 'a' - 'A'; // ���Сд
						if (++count == 2)
							break;
					}
				}
				// �ٰ�card��������
				card[0] += 'a' - 'A';
				cards[cardCount++] = card;

				// Ȼ���ҳ��ˡ���
				out >> act;
				//ͳ�ƻ���ֵ
				int nx = Getnum(act);
				lastcnt = LiuJu[nx / 10][nx % 10];
				lastnx = nx;
				LiuJu[nx / 10][nx % 10] = ++cnt;
				// ����һ��act��
				for (int j = 0; j < cardCount; j++)
				{
					if (cards[j] == act)
					{
						// ȥ�������ƣ������һ�����������λ
						cards[j] = cards[--cardCount];
						break;
					}
				}
			}
			else if (act == "GANG")
			{
				
				// �ȿ�����һ�غ���û���˺�
				{
					int tmp;
					string act, card;
					istringstream nextInput(input["requests"][i + 1].asString());
					if (nextInput >> tmp >> tmp >> act >> card && act == "HU" && card != "SELF")
						break;
				}

				//��������Ʊ����ջ����ƣ��ָ�����ֵ
				LiuJu[lastnx / 10][lastnx % 10] = lastcnt;
				// ��ʱ�Ҹ�����
				// ���������card����ΪСд����ʾ��
				for (int j = 0; j < cardCount; j++)
				{
					if (cards[j] == card)
						cards[j][0] += 'a' - 'A'; // ���Сд
				}
				// �ٰ�card��������
				card[0] += 'a' - 'A';
				cards[cardCount++] = card;
			}
			// HU�����ܳ��֣�PASS���ô���
			break;
		case 4:
			// ������������ܳ���
			;
		}
	}

	// �����Լ����غ�����
	istringstream in(input["requests"][turnID].asString());
	int type, who;
	string act, card, temp, myAction = "PASS";
	in >> type;
	if (type == 2)
	{
		// �ֵ�������
		in >> card;
		--paichi[Getnum(card)];
		
		//��������
		cards[cardCount++] = card;

		// �ܲ��ܺ���
		if (Hu()) // ע���ʱ���Ѿ�����
			myAction = "HU";
		else
		{
			SortHu();
			//��������
			cardCount_2 = cardCount;
			for (int i = 0; i < cardCount; ++i)
				cards_2[i] = cards[i];
			//���㲻�ܵĹ���
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
						// ȥ�������ƣ������һ�����������λ
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
				//�ָ�����
				card = Getstr(a * 10 + b);
				cardCount = cardCount_2;
				for (int i = 0; i < cardCount; ++i)
				{
					cards[i] = cards_2[i];
					if (cards[i][0] == card[0] && cards[i][1] == card[1]) cards[i][0] += 'a' - 'A';
				}
				//����ܵĹ���
				FindBest ();
				//�����������ǲ��Ǹ���
				if (maxvalue > GangValue)
				{
					GangValue = maxvalue;
					GangCard = card;
				}
			}
			//���׸ܲ��ܣ�
			if (GangValue > oldvalue * 0.99)
				myAction = "GANG " + GangCard;
		}	
	}
	else if (type == 3)
	{
		// ������ҡ���
		in >> who >> act >> card;

		if (act == "PLAY") // ���Ǳ��˴��ƣ�����ʲôҲ�ɲ���
		{
			// ���ս���

			// ��ʾ��
			// ���ֻ��PASS��
			// ����Ҳ���ûָ���
			// ��Ϊ�´λ����¼���
			--paichi[Getnum(card)];
			for (int i = 0; i < cardCount; i++) cards_2[i] = cards[i];
			cardCount_2 = cardCount;
			cards[cardCount++] = card;
			// �ܲ��ܺ���
			if (Hu()) // ע���ʱ���Ѿ�����
				myAction = "HU";
			else
			{
			// �ܲ��ܸ�/����
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
				//����Ҫ��Ҫ������
				if (count + count_2 >= 3) 
				{
					double oldvalue = -1, PengValue = -1, GangValue = -1;
					//�ָ�ԭ�����ƣ����������˴�����ţ�
					for (int i = 0; i < cardCount_2; i++) cards[i] = cards_2[i];
					cardCount = cardCount_2;
					//���㲻�������ܵĹ���
					FindBest ();
					oldvalue = maxvalue;
					bool flag = 1;
					//��������ƶ�Ͳ�����
					
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
					//����
					if (count >= 3 && flag)
					{
						//�ָ�ԭ�����ƣ����������˴�����ţ�
						for (int i = 0; i < cardCount_2; i++) cards[i] = cards_2[i];
						cardCount = cardCount_2;
						//�ѱ��˴�������Ƽ�������
						cards[cardCount++] = card;
						int PengCount = 0;
						for (int i = 0; i < cardCount; i++)
						{
							if (cards[i] == card) cards[i] = temp, PengCount++;
							if (PengCount == 3) break;
						}

						//�������Ĺ���
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
							//�ѱ��˴�������Ƽ�������
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
									// ȥ�������ƣ������һ�����������λ
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
					//�ܣ�
					if (count >= 4)
					{
						//�ָ�ԭ�����ƣ����������˴�����ţ�
						for (int i = 0; i < cardCount_2; i++) cards[i] = cards_2[i];
						cardCount = cardCount_2;
						//�ѱ��˴�������Ƽ�������
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
		// ����������TAT
		// ��һ��Ŀ������
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
	// ��������ֻ��PASS

	// �������JSON
	Json::Value ret;
	ret["response"] = myAction;
	ret["data"] = "While (1) ez_RP++";
	Json::FastWriter writer;
	cout << writer.write(ret) << endl;
	return 0;
}
