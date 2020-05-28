/*
* Perfect Cube
* 张雨泽 1900094801 
* 2020/04/26
*/
#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

int main()
{
	int N;
    cin >> N;
    for( int i = 6; i <= N; i++)
    {
        for( int a = 2; a < N; a++)
            for( int b = a; b < N; b++)
                for( int c = b; c < N; c++)
                {
                    int sum = a * a *a + b*b*b + c*c*c;
                    int cube = i * i *i;
                    if( sum > cube) break;
                    if( sum == cube)
                    {
                        printf("Cube = %d, Triple = (%d,%d,%d)\n", i, a, b, c);
                        break;
                    }
                }
    }
    system("pause");
    return 0;
} 
