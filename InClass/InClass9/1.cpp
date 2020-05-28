/*
张雨泽1900094801
A:扫雷游戏地雷数计算
2020/05/07
*/

#include <iostream>
using namespace std;

int g[150][150] = {0};
int n, m;

void addcnt(int i, int j)
{
    int dx[8] = {1,1,1,-1,-1,-1,0,0}, dy[8]={1,0,-1,1,0,-1,1,-1}; 
    for (int k = 0; k <8 ; k++)
    {
        if(i + dx[k] >= 0 && i + dx[k] < m && j + dy[k] >= 0 && j + dy[k] < n )
            g[dx[k] + i][dy[k] + j] += 1;
    }
}

int main()
{
    cin >> m >> n;
    for( int i = 0; i < m; i ++)
        for( int j = 0; j < n; j++)
        {
            char ch;
            cin >> ch;
            if (ch == '*')
            {
                g[i][j] = -1; 
                addcnt(i, j);
            }
        }

    for( int i = 0; i < m; i ++)
    {
        for( int j = 0; j < n; j++)
        {
            if(g[i][j] == -1)
            cout << "*";
            else cout << g[i][j];
            
        }
        cout << endl;
    }
    
    system("pause");
}