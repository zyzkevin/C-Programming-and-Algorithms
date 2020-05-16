/*
* 河中跳房子
* 张雨泽 1900094801 
* 2020/04/30
*/
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>
using namespace std;

int D[500001] = {0};
int N;

bool check( int dist, int N, int M)
{
    int cnt = 0; //number of splits
    int pos = 0;
    for( int i = 0; i <= N; i++)
    {
        if( D[i] - pos < dist)
        {
            cnt++;
        }
        else
            pos = D[i];
    }

    if( cnt > M) return false;
    else return true;
}

int main()
{
    int M, L;
    cin >>L >>  N >> M;
    
    for(int i = 0; i < N; i++)
    {
        cin >> D[i];
    }
    
    D[N] = L;

    int s = 1, e = L, mid;
    int last_pos = -1;

    
    while( s <= e)
    {
        mid = s + (e - s) / 2;
        
        if( check( mid, N, M))
        {
            s = mid + 1;
            
            last_pos = mid;
        }
        else 
        {
            e = mid - 1;
        }
    }
    printf("%d\n", last_pos);
 
    system("pause");
    return 0;
} 
