#include <iostream>

using namespace std;

template <class T1, class T2>
class f
{
    T1 val; 
    public:
    f( T1 x): val(x) {}
    T2 operator () (T2 arg2)
    {
        return arg2 + val;
    }
};

int main()
{
   cout << f<int,int>(7)(9) << endl;   //16
   cout << f<string,string> (" hello!")("world")  <<endl; // world hello!
   cout << f<char,string> ('!')("world") << endl;
   
   system("pause");
   return 0;    //world!
}