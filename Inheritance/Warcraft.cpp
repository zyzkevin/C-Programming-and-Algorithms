#include <iostream>
#include <cstring>
#include <iomanip>
#include <stdio.h>
using namespace std;

class HQ;

class Warrior
{
    public:
    int no;
    int health;
    Warrior(){};
    Warrior( int health, int tribe, int no){
    };
    friend HQ;
};
//sword bomb arrow 0, 1, 2

class ninja: public Warrior
{
    int weapon1, weapon2;
    public:
    ninja (int h, int tribe, int warriorNo): Warrior( h, tribe, warriorNo), weapon1(warriorNo % 3), weapon2((warriorNo + 1) % 3)
    {} 
};
class dragon: public Warrior
{
    int weapon;
    float morale;
    public:
    dragon (int h, int tribe, int warriorNo, int hqH): Warrior( h, tribe, warriorNo), weapon(warriorNo % 3)
    { 
        if (h == 0) morale = 0;
        else
        morale = hqH / h;
    } 
};
class iceman: public Warrior
{
    int weapon1;
    public:
    iceman (int h, int tribe, int warriorNo): Warrior( h, tribe, warriorNo), weapon1(warriorNo % 3)
    {} 
};
class wolf: public Warrior
{
    public:
    wolf( int health, int tribe, int no): Warrior( health, tribe, no){}
};
class lion: public Warrior
{
    int loyalty;
    public:
    lion (int h, int tribe, int warriorNo, int hqH): Warrior( h, tribe, warriorNo), loyalty( hqH) {}
};

class HQ
{
    static char name[5][7];
    static char color[2][7];
    static char weaponName[3][7];

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
    int M; // health.
    public:
    HQ (int health, int side) : M(health), side(side) 
    {
        strcpy(col, color[side]);
        
    }
    void makeWarrior(int t)
    {
        //testing if stopped
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
        //returning if h = 0;
        if (M < warH[t]) 
        {
            test = 1;
            return;
        }
        test = 0;


        M -= warH[t]; //making warrior and subtracting health
        // what warrior to make. //dragon, ninja, iceman, lion, wolf.
  
        switch (t)
        {
            case 0: 
            {
                dragon war(warH[t], t, nWarrior, M);
            }; break;
            case 1: {
                ninja war(warH[t], t, nWarrior);
            }; break;
            case 2: {
                iceman war(warH[t], t, nWarrior);
            }; break;
            case 3: {
                lion war(warH[t], t, nWarrior, M);
            }; break;
            case 4:{
                wolf war(warH[t], t, nWarrior); break;
            }
        }
        
        nWarrior++;
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
            switch(t)
            {
                case 0: printf( "It has a %s,and it's morale is %.2lf\n", weaponName[nWarrior % 3], double(M) / double(warH[t])); break;
                case 1: printf( "It has a %s and a % s\n", weaponName[nWarrior % 3], weaponName[(nWarrior + 1) % 3]); break;
                case 2: printf( "It has a %s\n", weaponName[nWarrior % 3]); break;
                case 3: printf( "It's loyalty is %d\n", M); break;
                case 4: ;break;
            }
        }
    }

    
};


int HQ :: warH[5] = {0};
char HQ :: name[5][7] = {"dragon", "ninja", "iceman", "lion", "wolf"};
char HQ :: color[2][7] = {"red", "blue"};
char HQ :: weaponName[3][7] = {"sword", "bomb", "arrow"};

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
                if (rcnt >= 50) Red.stop = 1; // needs improvement
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
                        if (bcnt >= 50) 
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