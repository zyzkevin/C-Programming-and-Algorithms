#include <iostream>

using namespace std;
class Warrior
{
    public:
    int no = 1;
};

class Dragon:public Warrior
{
    public:
    Dragon ()
    {
        no = 3;
    }
};

int main()
{
    Warrior *p;
    p = new Dragon;
    cout << p -> no; 
    delete p;
    system("pause");
}