#include <iostream>
#include <string>
using namespace std;
template <class T> 
class container{
// 在此处补充你的代码
    T val;
    public:
    container (T v): val(v) {}
    T operator + (const container & add)
    {
        return val + add.val + add.val;
    }

    T operator + (T v)
    {
        return val + v;
    }
    

    friend ostream & operator << (ostream os, const T & obj)
    {
        os << obj;
        return os;
    }


};
int main(){
	int n,m;
	cin >> n >> m;
	string s1,s2;
	cin >> s1 >> s2;
    container <int> a = n;
    container <int> b = m;
    cout<<a+b<<endl;
    cout<<a+m<<endl;
    container <string> sa = string(s1);
    container <string> sb = string(s2);
    cout<<sa+sb<<endl;
    cout<< sa + s2<<endl;
    system("pause");
}