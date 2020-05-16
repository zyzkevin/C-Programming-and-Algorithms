#include <iostream>
using namespace std;

int main()
{
    char str[200];
    char res [50];
    while(cin >> str)
    {
        if(str == "caption:") 
        while (cin >> res && res != "\"img_url\":")
        {
            cout << res << endl;
        }
    }
}