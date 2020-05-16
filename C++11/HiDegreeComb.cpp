#include <iostream>
using namespace std;
// 在此处补充你的代码

template <class T1, class T2, class T3>
class combine
{
    T1 op1; T2 op2;
    public:
    combine(const T1 & op1, const T2 & op2): op1(op1), op2(op2)
    {

    }
    T3 operator() (T3 val)
    {
        return op1( op1(val) + op2(val));
    }
};

int main()
{
    auto Square = [] (double a) { return a * a; };
    auto Inc = [] (double a) { return a + 1; };
    cout << combine<decltype(Square),decltype(Inc),int>(Square,Inc)(3) << endl;
    cout << combine<decltype(Inc),decltype(Square),double>(Inc,Square)(2.5) << endl;
    system("pause");
    return 0;
}