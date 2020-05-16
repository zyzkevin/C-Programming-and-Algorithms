#include <iostream>
#include <map>
#include <vector>

using namespace std;

int main()
{
    map <int , int> a; // value, id
    map <int, int> :: iterator it;
    int n, id, val, idc, vc;
    cin >> n;
    a.insert({1000000000, 1});
    for( int i = 0; i < n; i++)
    {
        cin >> id >> val;
        
        it = a.lower_bound(val);
        if( it == a.end()) it --;

        vc = it -> first; idc = it -> second;
        int min = abs(vc - val); 
        if( it != a.begin()) it --;
        if( abs(val - it -> first) < min || (min == abs(it -> first - val) &&  it -> second < idc))
        {
            vc = it -> first;
            idc = it -> second;
            min = abs(val - vc);
        }
        cout << id << " " << idc << endl;
        it = a.find(val);
        if( it == a.end())
            a.insert({val, id});
        else if( id < it -> second )
            it -> second = id;
    }
    system("pause");
}