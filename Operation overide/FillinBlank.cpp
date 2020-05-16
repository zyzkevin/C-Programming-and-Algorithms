#include <iostream>
#include <cstring>
using namespace std;

class Array2 {
    int m, n; //m行n列
    int** ptr; 

    public:
    Array2 (const int r, const int c): m(r), n(c)
    {
        ptr = new int*[r];
        for (int i = 0; i < r; i++)
            ptr[i] = new int[c];
    }
    Array2 ()
    {
        ptr = NULL;
    }
    ~Array2 ()
    {
        for (int i = 0; i < m; i++)
        {
            delete [] ptr[i];
        }
    }
    Array2 & operator = (const Array2 a)
    {
        delete [] ptr; 
        m = a.m; n = a.n;
        ptr = new int*[m];
        for (int i = 0; i < m; i++)
            ptr[i] = new int[n];
        for (int i = 0; i < m; i++)
            for(int j = 0; j < n; j++)
                ptr[i][j] = a.ptr[i][j];
        return * this;
            
    }
    int * operator [] (int i)
    {
        return ptr[i];
    }

    int operator () (int i, int j)
    {
        return ptr[i][j];
    }

    
};

int main() {
    Array2 a(3,4);
    int i,j;
    for(  i = 0;i < 3; ++i )
        for(  j = 0; j < 4; j ++ )
            a[i][j] = i * 4 + j;
    for(  i = 0;i < 3; ++i ) {
        for(  j = 0; j < 4; j ++ ) {
            cout << a(i,j) << ",";
        }
        cout << endl;
    }
    cout << "next" << endl;
    Array2 b;     b = a;
    for(  i = 0;i < 3; ++i ) {
        for(  j = 0; j < 4; j ++ ) {
            cout << b[i][j] << ",";
        }
        cout << endl;
    }
    return 0;
}