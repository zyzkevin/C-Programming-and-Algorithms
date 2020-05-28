#include <iostream>
#include <cmath>
using namespace std;
int main()
{
    int N;
    cin >> N;
    int ans = 999999999;
    for( int i = 1 ; i <= N; i++)
        for( int j = 1; j * i <= N; j++)
            for( int k = 1; k * i * j <= N; k++)
            {
                if( i * j * k == N)
                    ans = min( 2* k * j + 2* i * j + 2* i * k, ans );
            }
    cout << ans;
    system("pause");
    return 0;

}