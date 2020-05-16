/*
* Pie
* 张雨泽 1900094801 
* 2020/04/30
*/
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>
using namespace std;

double pies[100001] = {0};

double a = 0;

bool check( double area, double N, double F)
{
    int cnt = 0; //number of splits
    int temps = 0; 
    //memcpy(pietemp, pies, sizeof(pies));

    for( int i = 0; i < N; i++) //checking if splitting with max = split works.
    {
        cnt += pies[i] / area;
    }
    if( cnt >= F) return true;
    else return false;
}

int main()
{
	double N, F;
    double m = 0;
    cin >> N >> F;
    F++;
    for( int i = 0; i < N; i++)
    {
        cin >> pies[i];
        pies[i] *= pies[i];
        a += pies[i];
    }
    

    double L = 0, R = a /(F), mid;
    double last_pos = -1;

    
    while( R - L > 1e-6)
    {
        double mid = L + (R - L) / 2;
        
        if( check( mid, N, F))
        {
            L = mid;
            
            last_pos = mid;
        }
        else 
        {
            R = mid;
        }

    }
    printf("%0.3lf\n", last_pos * 3.14159265359);
 
    system("pause");
    return 0;
} 
