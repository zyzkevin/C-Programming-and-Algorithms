/*
张雨泽1900094801
C:全排列
2020/05/07
*/

#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <algorithm>
using namespace std;

class mycomp
{
    public:
    bool operator () (const string & a, const string & b)
    {
        int len = a.length();
        for (int i = 0; i < len; i++)
        {
            if( a[i] < b[i]) return true;
            if( a[i] > b[i]) return false;
        }
    }
};

set<string, mycomp> myset; //num, cnt
set<string, mycomp>::iterator it;
void swap(char & i, char & j)
{
    char temp = i; 
    i = j;
    j = temp;
}

int main()
{
    string c;
    cin >> c;

    int len = c.length();
    for(int i = 0; i < len; i++)
        for(int j = 0; j < len; j++)
        {
            it = myset.find(c);
            if( it == myset.end())
                myset.insert(c);
            swap(c[i], c[j]);
        }

    
    for( it = myset.begin(); it != myset.end(); it++)
        cout << *it << endl;

    system("pause");
    return 0;

}