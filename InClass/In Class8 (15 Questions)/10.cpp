#include <iostream>
#include <map>
using namespace std;
// 在此处补充你的代码
class A
{

    int num;
    public:
    static int count;
    A(){
        count++;
    };
    A(int a): num(a){
        count ++;
    }
    virtual ~A ()
    {
        cout << "A::destructor" << endl;
        //count --;
    }

    void operator delete (void * a) // this override delete operator calls destructor first.
    {
        count --;
    }
};

class B: public A
{
    public:
    B(){
        count ++;
    };
    B(int a): A(a){
        //count ++;
    }
    ~B ()
    {
        cout << "B::destructor" << endl;
        //count --;
    }
};

int A::count = 0;
void func(B b) {  }

int main()
{
	A a1(5),a2;
	cout << A::count << endl;
	B b1(4);
	cout << A::count << endl;
	func(b1);
	cout << A::count << endl;
	A * pa = new B(4);
	cout << A::count << endl;
	delete pa;
	cout << A::count << endl;
    system("pause");
	return 0;
}