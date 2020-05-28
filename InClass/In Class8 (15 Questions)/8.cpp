#include <iostream>
#include <string>
#include <bitset>
using namespace std;

int main()
{
    int t, n, i , j;
    cin >> t;
    while( t--)
    {
        cin >> n >> i >> j;
        bitset<16> bs1(n);

        bs1.flip(j);

        for(int it = i + 1; it < j - 1; it++)
            bs1.set(it, true);
            
        for( int it = 0; it < i - 1; it++)
            bs1.set(it, false);
        for( int it = j + 1; it < 16; it++)
            bs1.set(it, false);
        cout << hex << bs1.to_ulong() << endl;
    }   

    system("pause");
}