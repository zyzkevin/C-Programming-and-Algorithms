#include <iostream>
#include <cmath>
using namespace std;

int main()
{
    int N;
    int max_i, max_j;
    double a, b, max = 0.0;
    cin >> N >> a >> b;
    for (int i = N; i >= 0; i--)
    {
        for(int j = N; j >= 0; j--)
        {
            double temp = double(j) / double(i);
            if( temp < (a / b) && temp > max)
            {
                if(j % int(a)== 0 && i % int(b) == 0)
                    continue;
                max = temp;
                max_i = j; max_j = i;
                break;
            }
        }
    }
    
    for( int j = 2; j < max_i; j++)
    for( int i = 2; i < max_i; i++)
    {
        if( max_i % i == 0 && max_j % i == 0)
        {
            max_i /= i;
            max_j /= i;
        }
    }

    cout << max_i << " " << max_j << endl;
    system("pause");
    return 0;
}