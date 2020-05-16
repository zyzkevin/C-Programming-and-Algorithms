#include <iostream>
using namespace std;
class Sample {
public:
	int v;
    Sample (int i)
    {
        v = i;
    }
    Sample ()
    {

    }
    Sample (const Sample & c)
    {
        v = 2 + c.v;
    }

};
void PrintAndDouble(Sample o)
{
	cout << o.v;
	cout << endl;
}
int main()
{
	Sample a(5);
	Sample b = a;
	PrintAndDouble(b);
	Sample c = 20;
	PrintAndDouble(c);
	Sample d;
	d = a;
	cout << d.v;
    system("pause");
	return 0;
}