#include <iostream>
#include <cstring>
using namespace std;
template <int bitNum>
struct MyBitset 
{
	char a[bitNum/8+1];
	MyBitset() { memset(a,0,sizeof(a));};
	void Set(int i,int v) {
		char & c = a[i/8];
		int bp = i % 8;
		if( v ) 
			c |= (1 << bp);
		else 
			c &= ~(1 << bp);
	}
// 在此处补充你的代码

    class bitVal
    {
        char & c;
        char * str;
        
        public:
        int bp;
        bool val;
        bitVal( int i, char * s): str(s), c( s[i/8]), bp(i % 8)
        {
            val = c & (1 << bp);
        }
        void operator = (int v)
        {
            if( v ) 
            {
                c |= (1 << bp);
                val = true;
            }
            else 
            {
                c &= ~(1 << bp);
                val = false;
            }
        }

        bitVal operator = (const bitVal & obj)
        {
            if( obj.val) 
            {
                c |= (1 << bp);
                val = true;
            }
            else 
            {
                c &= ~(1 << bp);
                val = false;
            }
            return * this;
        }

        friend ostream & operator << ( ostream & os, const bitVal & obj )
        {
            if( obj.val) 
                os << 1;
            else os << 0;
            return os;
        }
    };

    bitVal operator [] (int i)
    {
        return bitVal(i, a);
    }

void Print() {
		for(int i = 0;i < bitNum; ++i) 
			cout << (*this) [i];
		cout << endl;
	}

};

int main()
{
	int n;
	int i,j,k,v;
	while( cin >>  n) {
		MyBitset<20> bs;
		for(int i = 0;i < n; ++i) {
			int t;
			cin >> t;
			bs.Set(t,1);
		}
		bs.Print();
		cin >> i >> j >> k >> v;
		bs[k] = v;
		bs[i] = bs[j] = bs[k];
		bs.Print();
		cin >> i >> j >> k >> v;
		bs[k] = v;
		(bs[i] = bs[j]) = bs[k];
		bs.Print();
	}
    system("pause");
	return 0;
}