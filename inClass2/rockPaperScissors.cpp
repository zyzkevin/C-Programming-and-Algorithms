#include <iostream>
using namespace std;

int main()
{
    int a[100], b[100];
    int n, na, nb, awin = 0, bwin = 0, tie = 0;
    cin >> n >> na >> nb;
    for( int i = 0; i < na; i++)
        cin >> a[i];
    for( int j = 0; j <nb; j++)
        cin >> b[j];
    
    
    for( int i = 0, j = 0, cnt = 0; cnt < n; i++, j++, cnt++ )
    {
        if( i == na) i = 0;
        if( j == nb) j = 0;

        if( a[i] == b[j]) tie++;
        else if( a[i] == 2 && b[j] == 5) awin++;
        else if( a[i] == 0 && b[j] == 5) bwin++;
        else if( a[i] == 5 && b[j] == 2) bwin++;
        else if( a[i] == 5 && b[j] == 0) awin++;
        else if ( a[i] == 0 && b[j] == 2) awin++;
        else if ( a[i] == 2 && b[j] == 0) bwin++;
    }
    if( awin > bwin) cout << "A";
    else if (awin == bwin) cout << "draw";
    else cout << "B";
    system("pause");
}