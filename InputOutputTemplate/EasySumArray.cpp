#include <iostream>
#include <string>
using namespace std;
template <class T>
T SumArray(const T * _first, const T * _last)
{
// 在此处补充你的代码

    T sum = *_first;
    _first++;
    for( ; _first != _last; _first++)
    {
        sum += *_first;
    }
    return sum;
}
int main() {
	string array[4] = { "Tom","Jack","Mary","John"};
	cout << SumArray(array,array+4) << endl;
	int a[4] = { 1, 2, 3, 4};  //提示：1+2+3+4 = 10
	cout << SumArray(a,a+4) << endl;
    system("pause");
	return 0;
}