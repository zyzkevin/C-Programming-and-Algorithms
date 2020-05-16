#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <cstdlib>
using namespace std;

class Student {
    char name[30];
    int age;
    int studentid;
    double gpa[4];
    double avg = 0;
    public:
        void input()
        {  
            cin.getline(name, 30, ',');
            cin >> age;
            cin.get();
            cin >> studentid;  
            for (int i = 0; i < 4; i++)
            {
                cin.get();
                cin >> gpa[i];
            }
        };
        void calculate()
        {
            for (int i = 0; i < 4; i++)
                avg += gpa[i];
            avg /= 4.0;
        };
        void output()
        {
            cout << name << "," << age << "," << studentid << "," << avg;
        };

};


int main() {
	Student student;        // 定义类的对象
	student.input();        // 输入数据
	student.calculate();    // 计算平均成绩
	student.output();       // 输出数据
    system("pause");
}