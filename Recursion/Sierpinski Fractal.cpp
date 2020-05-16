#include <iostream>
#include <cstring>
#include <iomanip>
#include <cmath>

using namespace std; 


int n, sum; 
char t[2050][2050]; 
 
void triangle(int s, int x, int y)
{
	if( s == 1)
	{
		t[x] [y] =' '; t[x] [y + 1] ='/'; t[x] [y+2] ='\\'; t[x] [y+3] =' '; 
		t[x + 1] [y] ='/'; t[x + 1] [y + 1] ='_'; t[x + 1] [y+2] ='_'; t[x + 1] [y + 3] ='\\'; 
		return; 
	}
    int temp = s - 1; 
    int temp2 = s + 1;
	for(int i = x; i < x + (1 << temp); i++)
	    for(int j = y; j < y + (1 << temp); j++) 
            t[i][j] =' '; 

	for(int i = x; i < x+(1 << temp); i++)
	    for(int j = y + ((1 << temp2)) - 1; y + ((1 << temp2))-j <= pow(2,  temp); j--)
            t[i][j] =' '; 

	triangle(temp, x + (1 << temp), y); 
	triangle(temp, x, y + pow(2,temp)); 
    triangle(temp, x + (1 << temp), y + pow(2, s)); 
}


int main()
{
    int n = 1; 
    while( n!= 0)
    {
        memset(t, 0, sizeof(t)); 
        cin >> n; 
        if( n== 0) break;
        sum = pow(2, n); 
        triangle(n,1,1);

		for(int i = 1; i <= sum; i++)
		{
			cout << t[i] + 1; 
            cout << endl;
		}
		cout << endl;
    }
    return 0; 
}