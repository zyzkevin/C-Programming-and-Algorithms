#include <iostream>
#include <cmath>
using namespace std;

bool isPrime(int x)
{
    for( int i = 2; i < int(sqrt(x)) + 1; i++)
    {
        if( x % i == 0)
            return false;
    }
    return true;
}

int swap(int x)
{
    int temp = 0;

    while(x > 0)
    {
        temp *= 10;
        temp += x % 10;
        x /= 10;
        
    }
    return temp;
}
int main()
{
    int n, m, flag = 0;
    cin >> n >> m;
    for (int i = n; i <= m; i++)
        if(isPrime(i))
            if(isPrime(swap(i)))
            {
                
                if( flag == 0)
                    cout << i;
                else cout << "," << i;
                flag = 1;
            }

    if (flag == 0) cout << "No" << endl;
    system("pause");
    return 0;
}