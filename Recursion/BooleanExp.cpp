#include <iostream>
#include <cstring>
using namespace std;
bool term();

bool exp()
{
    bool v = term();
    char ch;
    while( true)
    {
        char c;
        while( 1)
        {
            c = cin.peek();
            if (c == ' ')
                cin.get();
            else break;
        }
        if( c == '|')
        {
            cin >> ch;
            bool temp = term();
            if( v || temp)
                v = true;
            else 
                v = false;
        }
        else if( c == '&')
        {
            cin >> ch;
            bool temp = term();
            if (v && temp)
                v = true;
            else v = false;
        }
        else break;
    }
    return v;
}



bool term()
{
    bool v;
    char ch;
    char c;
    while( 1)
    {
        c = cin.peek();
        if (c == ' ')
            cin.get();
        else break;
    }

    if( c == '(')
    {
        cin >> ch;
        v = exp();
        cin >> ch;
    }
    else if( c== '!')
    {
        cin >> ch;
        v = !term();
    }
    else 
    {
        cin >> ch;
        if( ch == 'V')
            v = true; 
        else 
            v = false;
    }
    return v;
}

int main()
{
    char str[101] = {0};
    int num = 1;
    char ch;
    while( cin.get(ch))
    {
        if(ch == '\n') break;
        cin.putback(ch);

        if( exp())
            cout << "Expression " << num << ": " << "V" << endl;
        else
            cout << "Expression " << num << ": " << "F" << endl;

        //cin.get(); 
        num++;
        cin.get();
    }
    system("pause");
}