#include <iostream>
#include <memory>
#include <cstring>

using namespace std;
void flip( int * c, int i)
{
    if( c[i]) c[i] = 0;
    else c[i] = 1;
}

int main()
{
    char s1[31];
    char s2[31];
    int flag = 0;
    int lock[31];
    int ori[31], tar[31];
    int res[31] = {0};

    cin.getline(s1, 31);
    cin.getline(s2, 31);
    int size = strlen(s1);
    for( int i = 0; i < size; i++)
    {
        ori[i] = s1[i] - '0';
        tar[i] = s2[i] - '0';
    }

    int press = 0;
    int cnt = 0;
    int min = 99999999;
   
    for( int n = 0; n < 2; n++)
    {
        cnt = 0;
        memcpy(lock, ori, sizeof(ori));
        press = n;
        for( int i = 0; i < size; i++)
        {
            res[i] = press;
            if( press)
            {
                cnt++;
                if( i > 0)
                    flip(lock, i - 1);
                flip(lock, i);
                if( i < size - 1)
                    flip( lock, i + 1);
            }
            if(lock[i] != tar[i])
                press = 1;
            else press = 0;
        }
        if( lock[size - 1] == tar[size - 1])
        {
            flag = 1;
            if(cnt < min) min = cnt;
        }
    }

    if( flag == 0) cout << "impossible" << endl;
    else cout << min << endl;
    system("pause");
    return 0;
}