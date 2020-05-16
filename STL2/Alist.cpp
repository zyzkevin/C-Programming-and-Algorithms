#include <iostream>
#include <list>

using namespace std;

template <class T>
void printList(const list<T> & lst)
{
    int max = lst.size();
    if( max > 0)
    {
        typename list<T>:: const_iterator i;
        i = lst.begin();
        for( i = lst.begin(); i != lst.end(); ++i)
        {
            cout << * i << " ";
        }
       
    }
    cout << endl;
}

int main()
{
    int n = 0;
    cin >> n;
    char command[100];
    int arg1 = 0, arg2 = 0;
    list <int>* lts[10005];
    for (int i = 0; i < n; i++)
    {
        cin >> command >> arg1;
        switch (command[0])
        {
            case 'n': lts[arg1] = new list <int>; break;
            case 'o': 
            {
                lts[arg1] -> sort();
                printList(*lts[arg1]);
            } break;
            case 'u': 
            {
                lts[arg1] -> sort();
                lts[arg1] -> unique(); 
            } break;
            case 'm':
            {
                cin >> arg2;
                lts[arg1] -> merge(*lts[arg2]);
            } break;
            case 'a':
            {
                cin >> arg2;
                lts[arg1] -> push_back(arg2);
            } break;

        }
    }
    //delete [] lts;
    system("pause");
    return 0;
}