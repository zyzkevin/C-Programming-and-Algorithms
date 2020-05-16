#include <iostream>
#include <set>

using namespace std;
int main()
{
    int n = 0;
    cin >> n;
    char command[100];
    int x;
    multiset <int> s1, his;
    multiset<int>::iterator it;
    for (int i = 0; i < n; i++)
    {
        cin >> command >> x;
        switch (command[2])
        {
            case 'd': //add
            {
                s1.insert(x);
                his.insert(x);
                cout << s1.count(x) << endl;
            } break;

            case 'k': //ask
            {
                it = his.find(x);
                if( it != his.end())
                    cout << 1 << " ";
                else cout << 0 << " ";
                cout << s1.count(x) << endl;
            } break;

            case 'l': //del
            {
                int cnt = s1.count(x);
                cout << cnt << endl;
                for( int i = 0; i < cnt; i++)
                {
                    it = s1.find(x);
                    s1.erase(it);
                }
            } break;
        }
    }
}