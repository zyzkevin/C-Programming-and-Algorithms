/*
张雨泽1900094801
B:谁是你的潜在朋友
2020/05/07
*/

#include <iostream>
#include <queue>
#include <map>
using namespace std;


map<int, int> mp; //num, cnt
map<int, int>::iterator it;
int main()
{
    int n, m ;
    cin >> n >> m;
    queue<int> q;
    
    for(int i = 0; i < n; i++)
    {
        int temp;
        cin >> temp;
        q.push(temp);
        it = mp.find(temp);
        if( it != mp.end())
            it -> second++;
        else
        mp.insert(make_pair(temp, 0));
    }

    while(!q.empty())
    {
        int t = q.front();
        it = mp.find(t);
        if(it -> second != 0)
            cout << it -> second;
        else cout << "BeiJu";
        cout << endl;
        q.pop();
    }
    system("pause");
    return 0;

}