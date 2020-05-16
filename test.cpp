#include <iostream>

using namespace std;


int getDiff(int w)  //¼ÆËãÐèÒª²¹ÆëµÄÎ»Êý   
{
    int line = (w*32+31)/8;
	line-=line%4;
	return (line - w*4);

}

int make4( int a)
{
    int l = (a * 32) / 8;
    return 4 - (l % 4);
    
}


int main()
{
    int n;
    while(cin >> n)
    {

    cout << getDiff(n) << " " << make4(n) << endl;
    }
    system("pause");
    return 0;
}