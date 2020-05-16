#include<iostream>
#include<iomanip>
#include<cmath>
#include <algorithm>
using namespace std;
int n;
int nbond[100][10050] = {-9999};
int worker[2][100] = {0}; //0 is bond, 1 is ability
int his[1000] = {0};
int maxcnt;

int select(int sBo, int sAb, int cnt)
{
    if (cnt == n)
    {
        if( sBo <= 0) return -9999;
        if( sAb > maxcnt) maxcnt = sAb;
        return sAb;
    }

    //already came here.
    if (nbond[cnt][sBo + 6666] != -9999)
    {
        sAb += nbond[cnt][sBo + 6666];
        if(sAb > maxcnt) maxcnt = sAb;
        return sAb;
    }

    his[cnt] = 0;
    while (his[cnt] < 2){

       if(his[cnt]) 
       {
        sAb += worker[1][cnt];
        sBo += worker[0][cnt];
       }
       
       int child = select(sBo, sAb, cnt + 1);

       //backtracking
        if(his[cnt]) 
       {
        sAb -= worker[1][cnt];
        sBo -= worker[0][cnt];
       }

       child -= sAb;
       if(child > nbond[cnt][sBo + 6666]){
           nbond[cnt][sBo + 6666] = child;
       }
       his[cnt]++;
   }
   return sAb + nbond[cnt][sBo + 6666];
}

int main(void)
{
    cin >>n;
    //bond
    for( int i = 0; i < n; i++)
        cin >> worker[0][i];
    //ability
    for( int i = 0; i < n; i++)
        cin >> worker[1][i];
    //deleting both negative
    for( int i = 0; i < 100; i++)
        for( int j = 0; j < 10001; j++)
        {
            nbond[i][j] = -9999;       
        } 
    select(0, 0, 0);
    cout << maxcnt;
    system("pause");
	return 0;
}
