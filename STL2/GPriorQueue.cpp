#include <iostream>
#include <queue>
#include <cmath>

using namespace std;

bool isPrime(int a)
    {
        int bound = sqrt(a) + 1;
        for(int i = 2; i < bound; i++)
            if (a % i == 0)
                return false;
        return true;
    }

int getFactors(int a)
{
    int bound = sqrt(a);
    int cnt = 0;

    for( int i = 2; i <= bound; i++)
        if( a % i == 0)
        {
            if( isPrime(i))
                cnt++;
            if( isPrime(a/i))
                cnt++;
        }
    return cnt;
}

struct myfirst
{
    bool operator () (const int & a, const int & b)
    {
        if (getFactors(a) > getFactors(b))
            return false;
        else if( getFactors(a) == getFactors(b))
            return a < b;
        else return true;
    }
};

struct mylast
{
    bool operator () (const int & a, const int & b)
    {
        if (getFactors(a) > getFactors(b))
            return true;
        else if( getFactors(a) == getFactors(b))
            return a > b;
        else return false;
    }
};

int main()
{
    typedef priority_queue<int, vector<int>, myfirst> pq;
    typedef priority_queue<int, vector<int>, mylast> pq2;
    pq a;
    pq2 b;
    
    int num, e;
    cin >> num;

    while( num--){
        for( int i = 0; i < 10; i++)
        {
            cin >> e;
            a.push(e);
            b.push(e);
        }

        cout << a.top() << " " << b.top() << endl;
        a.pop();
        b.pop();
        //copying to new, and output last element

    }
    system("pause");
}