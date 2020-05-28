/*
张雨泽1900094801
C:怪盗基德的滑翔翼
2020/05/07
*/

#include <iostream>
#include <cmath>
#include <cstring>
using namespace std;


int h[10000] = {0};
int a[100] = {0}; // max num from i.
int b[100] = {0}; // gliding from right side.

int main ()
{
    int k, n;
    cin >> k;
    while( k--)
    {
        int lef = 1, rig = 1;
        cin >> n;
        memset(a, 0, sizeof(a)); memset(b, 0, sizeof(b));
        for( int i = 0; i < n; i++)
        {
            cin >> h[i];
            a[i] = 1; b[i] = 1;
        }
        for(int i = 0; i < n; i++)
        {
            for( int j = 0; j < i; j++)  
                if( h[j] >= h[i])         
                    a[i] = max(a[i], a[j] + 1);
            
            lef = max(lef, a[i]);
        }

        for(int i = 0; i < n; i++)
        {
            for( int j = 0; j < i; j++)  
                if( h[j] <= h[i])         
                    b[i] = max(b[i], b[j] + 1);
            
            rig = max(rig, b[i]);
        }


        cout << max(lef, rig)<< endl;

    }
    system("pause");
    return 0;
}