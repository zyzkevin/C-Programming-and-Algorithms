#include <iostream>
#include <algorithm>
#include <cstring>

using namespace std;
const int N = 4;

int board[N+1][N+1];

int ans_x = 0, ans_y = 0;

int terminal_state()
{
    int rowcnt[2][4] = {0};  // 0 records 1, 1 records -1
    int colcnt[2][4] = {0};
    int diagcnt[2] = {0};
    int odiagcnt[2] = {0};
    for( int i = 0; i < N; i++)
        for ( int j = 0; j < N; j++)
        {
            if(board[i][j] == 1) 
            {
                rowcnt[0][i]++;
                colcnt[0][j]++;
                if ( i == j) diagcnt[0]++;
                if( j == 3 - i) odiagcnt[0]++;
            }

            if(board[i][j] == -1) 
            {
                rowcnt[1][i]++;
                colcnt[1][j]++;
                if ( i == j) diagcnt[1]++;
                if( j == 3 - i) odiagcnt[1]++;
            }
        }
    
    for( int i = 0; i < 2; i++)
    {
        if(diagcnt[i] == 4 || odiagcnt[i] == 4)
        {
            if( i == 0) return 1; //x won
            else return -1; //x lost
        }
        for( int j = 0; j < 4; j++)
        {
            if( rowcnt[i][j] == 4 || colcnt[i][j] == 4)
            {
                if( i == 0) return 1;
                else return -1;
            }
            
        }
    }
    return 0;
}

int minimax( bool isMax, int a, int b)
{
    //if draw
    //if( gameOver && terminal_state == 0) return 0;

    int t = terminal_state();
    if( t != 0) return t;

    if( isMax == true)
    {
        int v = -9999;
        for( int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if( board[i][j] == 0) //available action
                {
                    
                    board[i][j] = 1; //taking actoin
                    int temp = minimax(false, a, b); //finding min.
                    board[i][j] = 0; //backtrack
                    v = max(v, temp);
                    if (v > a)
                    {
                        a = v;
                        ans_x = i;
                        ans_y = j;
                    }
                    if (b <= a)
                        return a;
                }
        return a;
    }
    else
    {
        int v = 9999;
        for( int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if( board[i][j] == 0) //available action
                {
                    
                    board[i][j] = -1; //taking actoin
                    int temp = minimax( true, a, b); //finding max.
                    board[i][j] = 0; //backtrack
                    v = min(temp, v);
                    b = min(b, v);
                    if (b <= a)
                        return b;
                }
        return b;
    }

}


int main ()
{
    char n, m;
    while (cin >> n && n != '$')
    {
        //clearing mem
        memset(board, 0 , sizeof(board));
        ans_x = 0; ans_y = 0;
        int cnt = 0;
        for (int i = 0; i < N; i++)
            for( int j = 0; j < N; j++)
            {
                cin >> m;
                switch (m)
                {
                    case 'x': board[i][j] = 1; break;
                    case 'o': board[i][j] = -1; break;
                    case '.': 
                    {board[i][j] = 0;
                    cnt ++;
                     } break;
                }
            }
        if (cnt >= 12)
        {
            cout << "#####" << endl;
            continue;
        }
        if( minimax( true, -9999, 9999) != 1) cout << "#####" << endl;
        else printf("(%d,%d)\n", ans_x, ans_y);
    }
}