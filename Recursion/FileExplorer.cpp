#include <iostream>
#include <cstring>
#include <iomanip>
#include <queue>

using namespace std;

char directories[200][30];
int i = 0;
void folders(int pos, int lvl)
{

    for( int j = pos; j < i; j++)
    {
        char c = directories[j][0];
        
    }
}


int main()
{
    string input;
    int n = 0;
    int lvl = 1;
    int rt = 1 ;
    priority_queue <string, vector<string>, greater<string> > q[101];
    while(1)
    {
        cin >> input;
        if( input[0] == '#') break;
        if( rt)
        {
            cout << "DATA SET " << ++n << ": " << endl;
            cout << "ROOT" << endl;
            rt = 0;
        }

        if(input[0] == 'f')
        {
            q[lvl].push(input);
        }

        else if( input[0] == 'd')
        {
            for(int i = 0; i < lvl; i++)
                cout << "|     ";
            cout << input << endl;
            lvl++;
        }

        else if(  input[0] == ']' || input[0] == '*')
        {
            while(!q[lvl].empty())
            {
                for(int i = 0; i < lvl - 1; i++)
                    cout << "|     ";
                cout << q[lvl].top() << endl;
                q[lvl].pop();
            }
            lvl--;
        }

    

        if(input[0] == '*')
        {
            rt = 1;
            lvl = 1;
            cout << endl;
        }
    }
    
    return 0;
}