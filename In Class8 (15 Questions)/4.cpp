#include <iostream>
using namespace std;
// 在此处补充你的代码
class CType
{

    public:
    int val;
    void setvalue(int n)
    {
        val = n;
    }

    int operator ++ (int x) {
        return val;
    }

    friend ostream &  operator << ( ostream & os, const CType & oj)
    {
        os << oj.val * oj.val;
        return os;
    }
};


int main(int argc, char* argv[]) {
	CType obj;
	int   n;		
	cin>>n;
	while ( n ) {
		obj.setvalue(n);
		cout<<obj++<<" "<<obj<<endl;
		cin>>n;
	}
	return 0;
}