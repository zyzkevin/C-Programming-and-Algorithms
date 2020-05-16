#include <iostream>
#include <map>

using namespace std;

int main()
{
    typedef map<int, int> M; //pow, id
    int n, id, pow, aid, apow;
    cin>> n;
    M::iterator it, l, r;
    int lef, rig;
    M a;
    a.insert({1000000000, 1});
    for( int i = 0; i < n;i ++)
    {
        cin >> id >> pow;
        
        it = a.lower_bound(pow);
        l = r = it;

        if( it != a.begin())
        {
            l--;
            lef = abs(l -> first - pow);
            rig = abs( r -> first - pow);
        }
        else
        {
            aid = it -> second;
            cout << id << " " << aid << endl;
            a.insert({pow, id});
            continue;
        }
        
        if (rig < lef)
            aid = r -> second;
        else
            aid = l -> second;

        cout << id << " " << aid << endl;
        a.insert({pow, id});
    }
    system("pause");
}