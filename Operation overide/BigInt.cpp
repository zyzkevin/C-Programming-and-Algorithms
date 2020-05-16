#include <iostream> 
#include <cstring> 
#include <cstdlib> 
#include <cstdio> 
using namespace std;
const int MAX = 110; 
class CHugeInt {
// 在此处补充你的代码
    char num[200];
    int n = 0;

    public:
    CHugeInt(const char * s)
    {
        strcpy(num, s);
    }
    CHugeInt(int n) : n(n) {
        num[0] = 0;
    }

    CHugeInt operator + (CHugeInt & b)
    {
        int len = strlen(num);
        char temp[200];
        int tmp[200]; //int
        for (int i = len - 1; i >= 0; i--)
            tmp[i] =  num[i] - '0';
        tmp[len - 1] += b.n; 
        for (int i = len - 1; i >= 1; i--) 
        {
            if (tmp[i] >= 10)
                {
                    tmp[i - 1] += tmp[i] / 10;
                    tmp[i] %= 10;
                }
        }
        while (tmp[0] >= 10)
        {
            for (int i = len - 1; i >= 0; i--)
                tmp[i + 1] = tmp[i];
            tmp[0] = tmp[1] / 10;
            tmp[1] %= 10;
            len++;
        }
        for (int i = 0; i < len; i++)
            temp[i] = tmp[i] + '0'; 
        temp[len] = 0;
        return CHugeInt(temp);
    }
    friend CHugeInt operator + (int j, CHugeInt & n)
    {
        int len = strlen(n.num);
        char temp[200];
        int tmp[200]; //int
        for (int i = len - 1; i >= 0; i--)
            tmp[i] =  n.num[i] - '0';
        tmp[len - 1] += j; 
        for (int i = len - 1; i >= 1; i--) 
        {
            if (tmp[i] >= 10)
                {
                    tmp[i - 1] += tmp[i] / 10;
                    tmp[i] %= 10;
                }
        }
        while (tmp[0] >= 10)
        {
            for (int i = len - 1; i >= 0; i--)
                tmp[i + 1] = tmp[i];
            tmp[0] = tmp[1] / 10;
            tmp[1] %= 10;
            len++;
        }
        for (int i = 0; i < len; i++)
            temp[i] = tmp[i] + '0'; 
        temp[len] = 0;

        return CHugeInt(temp);
    }
    CHugeInt operator + (int j )
    {
        int len = strlen(num);
        char temp[200];
        int tmp[200]; //int
        for (int i = len - 1; i >= 0; i--)
            tmp[i] =  num[i] - '0';
        tmp[len - 1] += j; 
        for (int i = len - 1; i >= 1; i--) 
        {
            if (tmp[i] >= 10)
                {
                    tmp[i - 1] += tmp[i] / 10;
                    tmp[i] %= 10;
                }
        }
        while (tmp[0] >= 10)
        {
            for (int i = len - 1; i >= 0; i--)
                tmp[i + 1] = tmp[i];
            tmp[0] = tmp[1] / 10;
            tmp[1] %= 10;
            len++;
        }
        for (int i = 0; i < len; i++)
            temp[i] = tmp[i] + '0'; 
        temp[len] = 0;
        return CHugeInt(temp);
    }
    
    friend ostream & operator << (ostream & os, CHugeInt obj)
    {
        if (obj.num[0] != 0)
            os << obj.num;
        else
            os << obj.n;
        return os;
    }
    CHugeInt & operator += (int j)
    {
        n += j;
        return *this;
    }
    CHugeInt & operator ++ ()
    {
        n ++;
        return *this;
    }
    CHugeInt operator ++ (int j)
    {
        CHugeInt temp (*this);
        n++;
        return temp;
    }


};
int  main() 
{ 
	char s[210];
	int n;

	while (cin >> s >> n) {
		CHugeInt a(s);
		CHugeInt b(n);

		cout << a + b << endl;
		cout << n + a << endl;
		cout << a + n << endl;
		b += n;
		cout  << ++ b << endl;
		cout << b++ << endl;
		cout << b << endl;
	}
	return 0;
}