#include <iostream>
#include <cstring>
#include <iomanip>
#include <stdio.h>
using namespace std;


class Warrior
{
    int h, strength;
    public:
    Warrior(int color, int health, int tribe): h(health){

    };
};

class HQ
{
    static char name[5][7];
    static char color[2][7];

    public:
    static int warH[5];

    char col[7];
    // dragon, ninja, iceman, lion, wolf.

    private:
    int side;
    int nWarrior = 0;
    
    int nTribe[5] = {0};
    public:
    int time = 0;
    int stop = 0;
    int test = 0;
    int hasStopped = 0;


    public:
    HQ (int health, int side) : M(health), side(side) 
    {
        strcpy(col, color[side]);
        
    }
    int M;


    void makeWarrior(int t)
    {

        if (stop)
        {
            if (!hasStopped)
            {
                hasStopped = 1;
                output(1, 1);
            }
            output(1, 1, 1);

            return;
        }

        if (M < warH[t]) 
        {
            test = 1;
            return;
        }
        
        test = 0;
        M -= warH[t];
        nWarrior++;
        Warrior nWarrior(1, warH[t], t);
        nTribe[t]++;
        output(t);
    }
    void printT()
    {
        cout << setw(3) << setfill('0') << time << " ";
    }
    void init();
    ~HQ();

    void output(int t, int end = 0, int hasStopped = 0)
    {
        if (hasStopped && end)
            return;
        else if (end)
        {
            printT();
            printf("%s headquarter stops making warriors\n", col);
        }
        else
        {
            printT();
            printf("%s %s %d born with strength %d,%d %s in %s headquarter\n", col, name[t], nWarrior, warH[t], nTribe[t], name[t], col);
            time++;
        }
    }

    
};


int HQ :: warH[5] = {0};
char HQ :: name[5][7] = {"dragon", "ninja", "iceman", "lion", "wolf"};
char HQ :: color[2][7] = {"red", "blue"};

int redl[5] = {2, 3, 4, 1, 0};
int bluel[5] = {3, 0, 1, 2, 4};

void HQ::init()
{
    nWarrior = 0;
    nTribe[5] = {0};
    time = 0;
    stop = 0;
    test = 0;
    hasStopped = 0;
}

HQ::~HQ()
{
}

int main()
{
    int n;
    cin >> n;
    for (int i = 1; i <= n; i++)
    {

        int M;
        cin >> M;
        HQ Red(M, 0);
        HQ Blue(M, 1);
        Red.init();
        Blue.init();
        for (int i = 0; i < 5; i++)
            cin >> Red.warH[i];
        //dragon, ninja, iceman, lion, wolf.
        cout << "Case:" << i << endl;
        int r = 0;
        int b = 0;
        int rcnt = 0;
        int bcnt = 0;
        while (!Red.stop || !Blue.stop)
        {
            if(r >= 5) r -= 5;
            if(b >= 5) b -= 5;
            Red.makeWarrior(redl[r]);
            if (Red.test && Red.stop == 0)
            {
                r++;
                rcnt++;
                if (r >= 5) r-= 5;
                if (rcnt >= 20) Red.stop = 1;
                continue;
            }
            Blue.makeWarrior(bluel[b]);
            if (Blue.test && Blue.stop == 0)
            {
                while (Blue.stop == 0 && Blue.test)
                {
                    Blue.makeWarrior(bluel[b]);
                    if (Blue.test && Blue.stop == 0)
                    {
                        b++;
                        bcnt++;
                        if (b >= 5) b-= 5;
                        if (bcnt >= 20) 
                        {
                            Blue.stop = 1;
                            Blue.makeWarrior(bluel[b]);
                        }
                        
                    }
                }
                
            }
            r++;
            b++;
        }
        Red.makeWarrior(1);
        Blue.makeWarrior(1);
    }
    system("pause");
}