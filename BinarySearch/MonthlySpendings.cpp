/*
* Monthly Spendings
* 张雨泽 1900094801 
* 2020/04/30
*/
#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

int spending[1001000] = {0};
int a = 0;

bool check( int split, int N, int M)
{
    int cnt = 1; //number of splits
    int temps = 0; 
    for( int i = 0; i < N; i++) //checking if splitting with max = split works.
    {
        if( spending[i] + temps > split )
        {
            cnt++;
            temps = spending[i];
        }
        else
        {
            temps += spending[i];
        }
    }
    if( cnt > M) return false;
    else return true;
}

int main()
{
	int N, M, m = 0;
    cin >> N >> M;
    for( int i = 0; i < N; i++)
    {
        cin >> spending[i];
        m = max(m, spending[i]);
        a += spending[i];
    }
    

    int L = m, R = a, mid;
    int last_pos = -1;
    
    while( L <= R)
    {
        int mid = L + (R - L) / 2;
        
        if( check( mid, N, M))
        {
            R = mid - 1;
            last_pos = mid;
        }
        else 
        {
            L = mid + 1;
        }

    }

    cout << last_pos << endl;

    system("pause");
    return 0;
} 
