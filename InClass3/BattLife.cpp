
#include <iostream>
#include <iomanip>
using namespace std;

int main()
{
    int N, b[1001], M = 0;
    while( cin >> N)
    {
        double sum = 0;
        for( int i = 0; i < N; i++)
        {
            cin >> b[i];

            //using up all bat = sum /2
            sum += b[i];
            M = max(M, b[i]);
        }
        if( M > sum - M)
            sum = sum - M;
        else
            sum /= 2.0;
        printf("%.1f\n", sum);
    }
    return 0;
}