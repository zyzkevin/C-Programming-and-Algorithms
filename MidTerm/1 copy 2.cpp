#include <iostream>
using namespace std;
int gcd(int a, int b) { // 计算两个数的最大公约数
    return (!b) ? a : gcd(b, a % b);
}
class Fraction {
    int p, q;
    public:
    // 在此处补充你的代码

    Fraction(){}
    Fraction(int a): p(a), q(1) {}
    Fraction(int a,int b): p(a), q(b) {}

    friend istream & operator >> (istream & is, Fraction & oj)
    {
        is >> oj.p >> oj.q;
        return is;
    }

    friend ostream & operator << (ostream & os, const Fraction & oj)
    {
        int d = gcd(oj.p, oj.q);
        os << oj.p /d;
        if( oj.q /d != 1) os << "/" << oj.q /d ;
        return os;
    }

    Fraction operator * (const Fraction & oj)
    {
        int newp = p * oj.p;
        int newq = q * oj.q;
        int d = gcd(newp, newq);
        return Fraction(newp/d, newq/d);
    }



};

int main(){
    int testcases;
    cin >> testcases;
    while (testcases --) {
        Fraction a, b, two(2);
        cin >> a >> b;
        cout << a << " * " << b << " = " << a * b << endl;
        cout << "2 * " << a << " = " << two * a << endl;
        cout << b << " * 3 = " << b * 3 << endl;
    }
    system("pause");
    return 0;
}