#include <cstdlib>
#include <iostream>
using namespace std;
int strlen(const char * s) 
{	int i = 0;
	for(; s[i]; ++i);
	return i;
}
void strcpy(char * d,const char * s)
{
	int i = 0;
	for( i = 0; s[i]; ++i)
		d[i] = s[i];
	d[i] = 0;
		
}
int strcmp(const char * s1,const char * s2)
{
	for(int i = 0; s1[i] && s2[i] ; ++i) {
		if( s1[i] < s2[i] )
			return -1;
		else if( s1[i] > s2[i])
			return 1;
	}
	return 0;
}
void strcat(char * d,const char * s)
{
	int len = strlen(d);
	strcpy(d+len,s);
}
class MyString
{
// 在此处补充你的代码
    char s[100];
    public: 
    MyString( char * p)
    {
        strcpy(s, p);
    }
    MyString()
    {
        s[0] = 0;
    }
    MyString( const MyString & str)
    {
        strcpy(s, str.s);
    }

    friend bool operator < (const MyString & s1, const MyString & s2)
    {
        if (strcmp(s1.s, s2.s) == -1) return true;
        else return false;
    }
    friend bool operator > (const MyString & s1, const MyString & s2)
    {
        if (strcmp(s1.s, s2.s) == 1) return true;
        else return false;
    }
    friend bool operator == (const MyString & s1, const MyString & s2)
    {
        if(strcmp(s1.s, s2.s) == 0) return true;
        else return false;
    }
    public:
    friend ostream & operator << ( ostream & os, const MyString & s)
    {
        os << s.s;
        return os;
    }
    /*friend ostream & operator << (const ostream & os, const char a[])
    {
        char temp[1000];
        strcpy(temp, a);
        os << temp;
        return os;
    }*/
    char & operator [] (int i)
    {
        return s[i];
    }

    MyString operator + (const MyString & str)
    {
        char temp[20];
        strcpy(temp, s);
        strcat(temp, str.s);
        return MyString(temp);
    }
    friend MyString operator + (const char str[], const MyString & c)
    {
        char temp[20];
        strcpy(temp, str);
        strcat(temp, c.s);
        return MyString(temp);
    }
    MyString operator = (const char *str)
    {
        strcpy(s, str);
        return *this;
    }
    
    MyString operator += (const char *str)
    {
        strcat(s, str);
        return *this;
    }

    MyString operator () (int i, int j)
    {
        char temp[20];
        strcpy(temp, s + i);
        *(temp + j) = 0;
        return MyString(temp);
    }
    

    friend int CompareString( const void * e1, const void * e2);
    friend void strcat(char * d,const char * s);
    friend int strcmp(const char * s1,const char * s2);
    friend void strcpy(char * d,const char * s);
    friend int strlen(const char * s) ;
};


int CompareString( const void * e1, const void * e2)
{
	MyString * s1 = (MyString * ) e1;
	MyString * s2 = (MyString * ) e2;
	if( * s1 < *s2 )
	return -1;
	else if( *s1 == *s2)
	return 0;
	else if( *s1 > *s2 )
	return 1;
}
int main()
{
	MyString s1("abcd-"),s2,s3("efgh-"),s4(s1);
	MyString SArray[4] = {"big","me","about","take"};
	cout << "1. " << s1 << s2 << s3<< s4<< endl;
	s4 = s3;
	s3 = s1 + s3;
	cout << "2. " << s1 << endl;
	cout << "3. " << s2 << endl;
	cout << "4. " << s3 << endl;
	cout << "5. " << s4 << endl;
	cout << "6. " << s1[2] << endl;
	s2 = s1;
	s1 = "ijkl-";
	s1[2] = 'A' ;
	cout << "7. " << s2 << endl;
	cout << "8. " << s1 << endl;
	s1 += "mnop";
	cout << "9. " << s1 << endl;
	s4 = "qrst-" + s2;
	cout << "10. " << s4 << endl;
	s1 = s2 + s4 + " uvw " + "xyz";
	cout << "11. " << s1 << endl;
	qsort(SArray,4,sizeof(MyString),CompareString);
	for( int i = 0;i < 4;i ++ )
	cout << SArray[i] << endl;
	//s1的从下标0开始长度为4的子串
	cout << s1(0,4) << endl;
	//s1的从下标5开始长度为10的子串
	cout << s1(5,10) << endl;
    system("pause");
	return 0;
}
