#include <iostream> 
using namespace std;
class A { 
	private: 
	int nVal; 
	public: 
	virtual void Fun() 
	{ cout << "A::Fun" << endl; }; 
	void Do() 
	{ cout << "A::Do" << endl; } 
}; 
class B:public A { 
	public: 
	void Do() 
	{ cout << "B::Do" << endl;} 
}; 
class C:public B { 
	public: 
	void Do( ) 
	{ cout <<"C::Do"<<endl; } 
	void Fun() 
	{ cout << "C::Fun" << endl; } 
}; 

void Call(
// 在此处补充你的代码
 B  &p

) { 
	p.Fun(); p.Do(); 
} 
int main() {  
	C c; 
	B b;
	Call( b); 
    system("pause");
	return 0;
}