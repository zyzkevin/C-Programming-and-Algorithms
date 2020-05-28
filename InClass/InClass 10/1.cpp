/*
张雨泽 1900094801
A:子串计算
2020/05/23

*/

#include <iostream>
#include <string>
#include <map>

using namespace std;

int len;
string a;

int main()
{
    map <string, int> mp; //substring, count;
    cin >> a;
    len = a.length();
    
    for(int i = 0; i < len; i++ ) //find all combinations of substrings, 
        for(int j = 1; j + i <= len; j--)
        {
            string tmp = a.substr(i, j);
            mp[tmp]++;
        }
    map <string, int>:: iterator it;

    for( it = mp.begin(); it != mp.end(); it++)
        if( it -> second > 1)
            cout<< it-> first << " " << it ->second << endl;
    

    system("pause");
    return 0;
}