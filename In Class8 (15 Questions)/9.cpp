#include <iostream>
#include <set>
#include <iterator>
#include <algorithm>
using namespace std;
// 在此处补充你的代码
class A
{
    public:
    int age;
    char group;
    A(int k): age(k), group('A'){}
};

class B: public A
{
    public:
    B(int k): A(k){
        group = 'B';
    }
};
//orders set by age.
struct Comp
{
    bool operator() (const A * x, const A * y)
    {
        return x -> age < y -> age;
    }
};

void Print (const A* p)
{
    cout << p -> group << " " << p->age << endl;
}



int main()
{
	int t;
	cin >> t;
	set<A*,Comp> ct;
	while( t -- ) {
		int n;
		cin >> n;
		ct.clear();
		for( int i = 0;i < n; ++i)	{
			char c; int k;
			cin >> c >> k;
			
			if( c == 'A')
				ct.insert(new A(k));
			else
				ct.insert(new B(k));
		}	
		for_each(ct.begin(),ct.end(),Print);
		cout << "****" << endl;
	}
    system("pause");
}