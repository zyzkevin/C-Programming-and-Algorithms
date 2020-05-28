/*
张雨泽1900094801
B:最佳加法表达式
2020/05/19
*/

#include <iostream>
#include <cmath>
#include <cstring>
using namespace std;


string sum (string a, string b)
{
    string tmp;
    int flag = 0;
    int al = a.length(), bl = b.length();
    for (int i = al - 1, j = bl - 1; i >= 0 || j >= 0; i--, j--)
    {
        //moving all digits right
        tmp = " " + tmp;
        int sum1 = 0, sum2 = 0;
        if(i >= 0)
            sum1 = a[i] - '0';
        if( j >= 0)
            sum2 = b[j] - '0';

        tmp[0] = sum1 + sum2 + flag;
        if( tmp[0] >= 10) {
            flag = 1; tmp[0] = tmp[0] - 10;
        }else {
            flag = 0;
        }
        tmp[0] = tmp[0] + '0';
    }
    //last flag needs to be seperately added
    if(flag)
        tmp = '1' + tmp;
    return tmp;
}

string mymin ( string a, string b)
{
    if( a.length() == b.length())
        return a < b ? a: b;
    return  a.length() < b.length() ? a : b;
}

int main()
{
    string dp[65][60]; //min of x numbers insert y plus signs
    string a;
    //cout << sum(string("9999"), string("44"));
    int m;
    while(cin >> m)
    {
        cin >> a;
        a= " " + a;
        //memset(dp, 0, sizeof(dp));
        int len = a.length();
        for( int i = 0; i <= len; i++)
            dp[i][0] = a.substr(1, i);
        
        for( int i = 1; i <= m; i++)
            for( int j = i + 1 ; j <= len; j++ )
                for( int k = i; k < j; k++)
                {
                    if(k == i) //no choice but to place + before second to last digit and add everything that's later. 
                          dp[j][i] = sum( dp[k][ i - 1], a.substr( k + 1,j - k));
                    else 
                    dp[j][i] = mymin( dp[j][i], sum(dp[k][i - 1], a.substr(k + 1, j - k)));
                }
        cout << dp[len][m]<< endl;
    }

    system("pause");
    return 0;
}