#include <iostream>
#include <iterator>
using namespace std;

template<class T1, class T2>
void Copy(T1 s, T1 e, T2 x) {
    for(; s != e; ++s, ++x)
        *x = *s;
}
// 在此处补充你的代码


class Fib
{  


    public:
        static int list[10000] = {0};
            int pos;
    Fib (){}

    
    void operator () (int i)
    {
        if( i == 1) {pos = 0; list[0] = 1; return;}
        if( i == 2) {pos = 1; list[0] = 1; return;}
        list[i] = list[i - 1] + list[ i - 2];
    }
    
    void operator ++ ()
    {
        pos++;
    }

    int operator * ()
    {
        return list[pos];
    }


    bool operator != (const Fib & obj) 
    {
        if( obj.pos != pos) return true;
        else return false;

    
    }
};

Fib::list[10000] = {0};

int main() {
	while(true) {
		int n;
		cin >> n;
		if(n == 0)
			break;
		
	    Fib f1(1), f2(n);
	    ostream_iterator<int> it(cout, " ");
	    Copy(f1, f2, it);
	    cout << endl;
	}
    system("pause");
	return 0;
}