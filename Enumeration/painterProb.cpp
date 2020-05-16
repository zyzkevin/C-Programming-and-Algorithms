#include <iostream>
#include <memory>
#include <cstring>


using namespace std;

int getBit( short c, int i)
{
    return (c >> i) & 1;
}

int oneCnt( short c)
{
    int n;
    for (n = 0; c > 0; ++n){
        c &= c - 1;
    }
    return n;
}
void setBit( short & c, int i, int v)
{
    if ( v)
        c |= (1 << i);
    else
        c &= ~(1 << i);
}

void flip( short & c, int i)
{
    c ^= (1 << i);
}

int main()
{
    int n;
    cin >> n;
    short grids[16] = {0};
    short origrids[16] = {0};
    short results[16] = {0};
    

    short row = 0;
    char ch;
    int min = 999999999;
    
    for( int i = 0; i < n; i++)
        for( int j = 0; j < n; j++)
        {
            cin >> ch;
            if( ch == 'w')
                setBit(origrids[i], j, 1);

            else setBit(origrids[i], j, 0);
        }

    int flag = 0;
    
    short i, j, k;

    for( i = 0; i < (1 << n); i++)
    {
        int cnt = 0;
        memcpy(grids, origrids, sizeof(origrids));
        row = i;
        for( j = 0 ; j < n; j++)
        {
            results[j] = row;
            cnt += oneCnt(row);
            for( k = 0; k < n; k ++)
            {
                if( getBit( row, k))
                {
                    if( k > 0) 
                        flip( grids[j], k - 1);

                    flip( grids[j], k);

                    if( k < n - 1)
                        flip( grids[j], k + 1);
                    //cnt++;
                }
            }

            if (j < n - 1) 
                grids[j + 1] ^= row;
            row = grids[j];
        
        }

        if( grids[n - 1] == 0) 
        {
            if( cnt < min) min = cnt;
            flag = 1;
            break;
        }
    }


    if (flag == 0) 
        cout << "inf";
    else 
        cout << min << endl;
    system("pause");
    return 0;
}