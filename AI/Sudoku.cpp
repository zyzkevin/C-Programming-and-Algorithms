#include <iostream>
#include <queue>
#include <iomanip>
#include <cstring>

using namespace std;
#define N 9

int sudoku[N][N] = {0};
int final[N][N] = {0};
void getGrid();
void print();
bool v[N][N + 1]; //row , num
bool grid[3][3][N + 1]; //grid, if num taken.

//d = [x - i + 8]
//ad = [x + i -1]

void dfs(int x, int num) //attempting column x == cnt of num, int num (num is 1 smaller than actual value)
{
    if (num == 0) print();
    if (x > 8) return;
    else{
        for (int i = 0; i < N; i++) // attempting row i;
        {
            //getGrid();
            //getting grid.
            //int flag = 0; // check if x==8.
            int gridi = i / 3;
            int gridj = x / 3;
            if (sudoku[i][x] == num) 
            {
                if (x == 8)
                {
                    num--;
                    x = -1;

                }
                dfs(x + 1, num);
            }
            if (!v[i][num] && !grid[gridi][gridj][num])
            {
                sudoku[i][x] = num;
                v[i][num] = true;
                grid[gridi][gridj][num] = true;

                if (x == 8)
                {
                    num--;
                    x = -1;

                }

                dfs(x + 1, num);

                if (x == -1)
                {
                    x = 8;
                    num++;
                }

                v[i][num] = false; 
                grid[gridi][gridj][num] = false;
                //sudoku[i][x] = 0;

            }
            
        }   
    }
}
void getGrid()
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            if (sudoku[i][j])
                {
                    v[i][sudoku[i][j]] = true;
                    grid[i/3][j/3][sudoku[i][j]] = true;
                }
        }
        
}
void print()
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            cout << sudoku[i][j];
            if (j == N - 1) cout << endl;
        }
}

int main()
{
    int t;
    cin >> t;
    int temp;
    for (int i = 0; i < t; i++)
    {
        memset(sudoku, 0, sizeof(sudoku));
        memset(grid, 0, sizeof(grid));
        memset(v, 0, sizeof(v));
        for (int j = 0; j < 9; j++)
        {
            cin >> temp;
            for (int k = 0; k < 9; k++)
            {
                sudoku[j][8 - k] = temp % 10;
                temp /= 10;
            }
        }
        getGrid();
        dfs(0, 9);
    }
    print();
        
    system("pause");
}