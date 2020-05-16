#include <iostream>
using namespace std;
class MyCin
{
// 在此处补充你的代码
    bool input = true;
    public:
    MyCin & operator >> ( int & n)
    {
        cin >> n;
        if (n == -1) input = false;
        
        return *this;
    }
    operator bool()
    {
        return input;
    }

};
int main()
{
    MyCin m;
    int n1,n2;
    while( m >> n1 >> n2) 
        cout  << n1 << " " << n2 << endl;
    system("pause");
    return 0;
}