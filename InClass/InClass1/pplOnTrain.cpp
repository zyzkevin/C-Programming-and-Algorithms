#include <iostream>
#include <cmath>
using namespace std;

int up[16] = {0};
int down[16] = {0};
int ppl[16] = {0};

int a;
int on(int s, int i)
{
    if( s == 1) 
    {
        ppl[s] = a;
        return a;
    }
    if( s == 2) 
    {
        ppl[s] = a;
        return i;
    }
    up[s] = on(s -2, i) + on( s- 1, i);
    down[s] = on( s- 1, i);
    ppl[s] = ppl[s-1] + on(s- 2, i);
    return up[s];
}

int main()
{
    int n, x, m; //a goes on first, n stations, m off last station, find x station get on.
    cin >> a >> n >> m >>x;
    //at m station, a + 
    for( int i = 0; i < m; i++)
    {
        on(n - 1, i);
        if( ppl[n - 1] == m)
        {
            break;
        }
    }
    cout << ppl[x] << endl;
    
    system("pause");
    return 0;
}