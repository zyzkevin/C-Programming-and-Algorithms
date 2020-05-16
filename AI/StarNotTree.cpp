#include <iostream>
#include <algorithm>
#include <cstring>
#include <random>
#include <iomanip>
#include <random>
using namespace std;

struct point
{
    double x;
    double y;
    point() {}
    point (double x, double y): x(x), y(y) {}
};

point comps[1000];
int N; //num of comps

double dir[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1},{0, -1}, {-1, -1}};


double sumOfNearest( const point & p)
{
    double sum = 0.0;
    for( int i = 0; i < N; i++)
    {
        sum += sqrt(((comps[i].x - p.x) * (comps[i].x - p.x)) + ((comps[i].y - p.y) * (comps[i].y - p.y)) );
    }
    return sum;
}

double simulated_annealing ()
{
    double v1 = rand() % 10000;
    double v2 = rand() % 10000;
    point next; 
    point cur(5000, 5000);
    double tmpAns;
    double curans = 9999999;
    for( double t = 1000; t > 1e-6; t *= 0.9)
    {
        for( int i = 0; i < 8; i++)
        {
            next.x = cur.x + dir[i][0] * t;
            next.y = cur.y + dir[i][1] * t;
            if( next.x > 10000 || next.y > 10000 || next.x < 0 || next.y < 0)
                continue;
            tmpAns = sumOfNearest(next);
            if (tmpAns < curans)
            {
                curans = tmpAns;
                cur = next;
                break;
            }
        } 
    }
    return curans;
}


int main ()
{
    cin >> N;
    for( int i = 0; i < N; i++)
    {
        cin >> comps[i].x >> comps[i].y;
    }
    cout << fixed << setprecision(0) <<  simulated_annealing();
    system("pause");
    return 0;
}