/*
张雨泽1900094801
A:滑雪
2020/05/07
*/

#include <iostream>
#include <cmath>
#include <cstring>
using namespace std;

int h[101][101] = {0}; //records height of slopes
int dx[4] = {1, -1, 0, 0}, dy[4] = { 0, 0, -1, 1};
int dp[101][101] = {0}; //records number of lower paths.

int n, m;

int slide (int i, int j) //returns max distance starting from i,j
{
    if (dp[i][j] != -1) return dp[i][j];
    dp[i][j] = 1; //1 including itself.
    for(int k = 0; k < 4; k++)
    {
        if( i + dx[k] >= 0 && j + dy[k] >= 0 && i + dx[k] < n && j + dy[k] < m)
            if( h[i + dx[k]][j + dy[k]] < h[i][j])
            {
                //go down or not
                dp[i][j] = max(dp[i][j], slide(i + dx[k], j + dy[k]) + 1);
            }
    }
    return dp[i][j];
}

int main()
{
    cin >> n >> m;
    int apex = 0, mi, mj; 
    for( int i = 0; i < n; i++)
        for( int j = 0; j < m; j++)
        {
            cin >> h[i][j];
            dp[i][j] = -1;
        }

    for (int i = 0; i < n; i++)
        for( int j = 0; j < m; j++)
            apex = max(slide(i, j), apex);
    cout << apex << endl;
    
    system("pause");
    return 0;
}