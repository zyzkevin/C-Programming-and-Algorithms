#include <iostream>
#include <cstring>
#include <iomanip>
#include <stdio.h>
#include <math.h>
using namespace std;


char name[5][7] = {"dragon", "ninja", "iceman", "lion", "wolf"};
int N, K, T, R;


int hour = 0;
int mins = 0;
int minCnt = 0;
int warEndb = 0;
int warEndr = 0;


void printT()
{
    cout << setw(3) << setfill('0') << hour << ':' << setw(2) << setfill('0') << mins << " ";
}

class HQ;
class Warrior;
class City;



class Weapon
{

    public:
    int used = 0;
    protected:
    Warrior * pOwner;
    
    friend class Warrior;
    
    Weapon (int i, Warrior * p);
    public:
    int No; // 0 sword, 1 , bomb, 2 arrow
    int damage;
    virtual ~Weapon() {}
    virtual int getDmg() // returns damage
    {
        return damage;
    }
    virtual void useWeapon () {}
    virtual void updateDmg () {}
    virtual void printWeap() {}
    virtual void newOwner( Warrior * p)
    {
        pOwner = p;
        updateDmg();
    }
};

class Sword: public Weapon
{
    public:
    Sword(Warrior * p);
    void updateDmg( int percent = 100);
    void printWeap();
    void useWeapon()
    {
        updateDmg(80);
    }
    
};

class Bomb: public Weapon
{
    public:
    Bomb(Warrior * p);
    void updateDmg ();
    void printWeap();
    void useWeapon(){}
};

class Arrow: public Weapon
{
    public:
    Arrow( Warrior * p);
    void updateDmg ();
    void printWeap();

    void useWeapon()
    {
        used++;
    }
};

class City
{
    public:
    Warrior * redWar = NULL; Warrior * blueWar = NULL;
    int CityNo;
    int deathCnt = 0;
    int whoStart = 0; //-1 red, 1 blue.
    int elements = 0;

    City(int i): CityNo(i){}
    void DragRoarTest();
    int flag = 0; //-1 red, 0 none, 1 blue.
    int last_won = 0; //-1 red, 1 blue.
    void printFlag( int i);
    void endBattle();
    void warTake(Warrior * p = NULL); 
    void fight();
    void deleteR(int warNo);
    void deleteB(int warNo);
    void arrowFight();
    void RaiseFlag(int );
    void bombCheck();
    void gainElements()
    {
        elements += 10;
    }

    void hqTake( HQ * hq);

    void DragonRoar( int side);
    int endFight = 1;
};

City * cities[24];


class HQ
{
    public:
    Warrior * warriors[1000];
    //int warrior_Alive[1000] = {0};
    static char color[2][7];
    static char weaponName[3][7];
    public:
    static int warH[5];
    static int warA[5];
    int side; // 0 red, 1 blue

    char col[7];
    // dragon, ninja, iceman, lion, wolf.
    private:
    

    public:
    int nWarrior = 0;

    int stop = 0;

    int M; // health.
    public:
    HQ (int health, int side) : M(health), side(side) 
    {
        strcpy(col, color[side]);
        
    }
    bool makeWarrior(int t);

    void warriorDeath( int warNo);
    
    void init();

    bool rewardWar()
    {
        if( M > 8) {
            M -= 8;
            return true;
        }
        else return false;
    }

    void collectHP(int i)
    {
        M += i;
    }


    void victory()
    {

        if( side) //blue hq
        {
            warEndb ++;
            if( warEndb == 2)
            {
                printT();
                printf("%s headquarter was taken\n", color[0]);
            }
        }
        else
        {
            warEndr ++;
            if( warEndr == 2)
            {
                printT();
                printf("%s headquarter was taken\n", color[1]);
            }
        }
    }
    void printHealth()
    {
        printT();
            printf("%d elements in %s headquarter\n", M, col);
    }
    
    void output(Warrior * p, int t, int end = 0, int hasStopped = 0);
};

class Warrior
{
    friend class City;
    protected:
    City* loc;

    
    public:
    double morale = 0.0;
    int nloc;
    int strength;
    int reachedHQ = 0;
    int preHp = 0;
    int no;
    int preSdmg = 0;
    HQ* side;
    int weapPos = 0;
    int type;
    int weapCnt = 0;
    int enem_die = 0;
    int hp;
    int steps = 0;
    int won = 0;
    int moved = 0;

    Weapon * pWeaps[3] = {NULL};

    // needs to be changed
    Warrior( int attack, int health, int no, HQ * color): strength(attack), hp(health), no(no), side(color) 
    {
        if( color -> side == 0)
        {
            loc = cities[0];
            nloc = 0;
            loc -> redWar = this;
        }
        else 
        {
            loc = cities[N - 1];
            nloc = N - 1;
            loc -> blueWar = this;
        };
    };

    Weapon * getWeapon( int warriorNo, Warrior *owner)
    {
        weapCnt ++;
        switch (warriorNo % 3)
        {
            case 0: 
                return new Sword(owner); break;
            case 1: 
                return new Bomb( owner); break;
            case 2: 
                return new Arrow( owner); break;
        }
    }

    void reportWeaps()
    {
        int scnt = -1; int bcnt = -1; int acnt = -1; 
        int sum = 0; //counts how many weapons, max 3
        for( int i = 0; i < 3; i++)
            if( pWeaps[i] != NULL)
                switch (pWeaps[i] -> No)
                {
                    case 0: scnt = i; sum++; break;
                    case 1: bcnt = i; sum++; break;
                    case 2: acnt = i; sum++; break;
                }
        printT();

        switch(sum)
        {
            case 0: printf( "%s %s %d has no weapon", side -> col, name[type], no); break;
            case 1: {
                printf( "%s %s %d has ", side -> col, name[type], no); 
                if(acnt != -1) pWeaps[acnt] -> printWeap(); 
                else if( bcnt != - 1) pWeaps[bcnt] -> printWeap(); 
                else if( scnt != - 1) pWeaps[scnt] -> printWeap(); 
            } break;
            default: {
                printf( "%s %s %d has ", side -> col, name[type], no);
                int flag = 0;
                if( acnt != - 1) { 
                    pWeaps[acnt] -> printWeap(); flag = 1;
                }
                if( bcnt != - 1) 
                { 
                    if(flag) cout << ",";
                    pWeaps[bcnt] -> printWeap(); 

                }
                if( scnt != - 1) {
                    cout << ",";
                    pWeaps[scnt] -> printWeap(); 
                }
            }
            break;
        }
                cout << "\n";
        

    }

    virtual void runAway() {}
    friend class HQ;

    virtual void hurt (int n, int arrow = 0){
        hp -= n;
        
        if (hp <= 0 && arrow == 0)
        {
            hp = 0;
            loc -> endBattle(); 
        }
    };

    virtual void counterA( Warrior * p)
    {
        //flatout attack
        if( won)
        {
            won = 0;
            onWin (p);
            return;
        }
        printT(); 
        printf("%s %s %d fought back against %s %s %d in city %d\n", side -> col, name[type], no, p -> side -> col, name[p ->type], p -> no, nloc);
        p -> hurt( strength / 2);
        if( weapCnt > 0)
        int weapPos;
        for( weapPos = 0; weapPos < 3; weapPos++)
                if( pWeaps[weapPos] != NULL)
                    if (pWeaps[weapPos] ->No == 0)
                    {
                        p ->hurt(pWeaps[weapPos] -> getDmg());
                        pWeaps[weapPos] ->useWeapon();
                        if( pWeaps[weapPos] -> damage == 0) {pWeaps[weapPos] = NULL;
                        weapCnt --;
                        }
                        break;
                    }
        if( won)
        {
            won = 0;
            onWin (p);
        }
        else
            lionLoseLoy();


        
    }


    void shootArrow(Warrior * p)
    {
        if( weapCnt > 0)
        {
            for( weapPos = 0; weapPos < 3; weapPos++)
                if( pWeaps[weapPos] != NULL)
                    if (pWeaps[weapPos] ->No == 2)
                        break;
            if ( weapPos == 3){
                weapPos = 0;
                return;
            }
            if ( pWeaps[weapPos] -> No == 2)
            {
                moved = 1;
                printT(); 
                printf("%s %s %d shot", side -> col, name[type], no);  
                p -> hurt(R, 1);
                if( pWeaps[weapPos] -> used == 2)
                {
                    delete pWeaps[weapPos];
                    pWeaps[weapPos] = NULL;
                    weapCnt --;
                }

                if( p-> hp <= 0) // enem died.
                {
                    printf(" and killed %s %s %d", p -> side -> col, name[p ->type], p -> no);
                    if( p -> side -> side == 0 && p -> loc -> blueWar != NULL)
                        p -> loc -> blueWar -> won = 1;
                    else if( p -> side -> side == 1 && p -> loc -> redWar != NULL)
                        p -> loc -> redWar -> won = 1;
                    
                    p -> hp = 0;
                    
                }

                cout << endl;

                if( pWeaps[weapPos] != NULL)
                {
                    pWeaps[weapPos] -> useWeapon();
                    if( pWeaps[weapPos] -> damage == 0) {
                        pWeaps[weapPos] = NULL;
                        weapCnt --;}
                }
            }
        }
    }

    void attack(Warrior * p)
    {    
        //        if(enem_die)

        if( won)
        {
            won = 0;
            onWin (p);
            return;
        }
        //if( p-> won) return;
        if( hp <= 0 || p -> won) 
            return;
        //    return;
        if( weapPos == 3) weapPos = 0;
        //find sword
        for( weapPos = 0; weapPos < 3; weapPos++)
                if( pWeaps[weapPos] != NULL)
                    if (pWeaps[weapPos] ->No == 0)
                        break;

        if( weapPos == 3 ) weapPos = 0;
            for( ; pWeaps[weapPos] == NULL; weapPos++)
                
        
        if( p == NULL || p -> hp == 0)
        {
            loc -> endBattle();
            enem_die = 1;
            return;
        } 

        printT(); 
        printf("%s %s %d attacked %s %s %d in city %d with %d elements and force %d\n", side -> col, name[type], no, p -> side -> col, name[p ->type], p -> no, nloc, hp, strength);  
        p -> hurt( strength);

        if( weapCnt > 0 )
        {
            switch( pWeaps[weapPos] -> No)
            {
                case 0: {
                    p -> hurt( pWeaps[weapPos] -> getDmg());  
                    pWeaps[weapPos] -> useWeapon();
                    if( pWeaps[weapPos] -> damage == 0){ pWeaps[weapPos] = NULL;
                    weapCnt --;}
                } break;
            }
        }

        if( won)
        {
            won = 0;
            onWin (p);
        }
        else
            lionLoseLoy();
            //weapPos++;
    }

    void useBomb( Warrior *p)
    {
        if(hp <= 0 || p -> hp <= 0) return;

        //if (moved) return;
        int hasSword = 0;
        int sumDmg = 0;
        int sworddmgself = 0;

        if( loc -> whoStart == -1 && side -> side == 0 || loc -> whoStart == 1 && side -> side == 1)
        {
            for( int i = 0 ; i < 3; i++)
                if( pWeaps[i] != NULL && pWeaps[i] -> No == 0)
                {
                    sworddmgself = pWeaps[i] -> damage; break;
                }
            if( strength + sworddmgself >= p -> hp)
            {
                return;
            }
        }

        if( weapCnt > 0 && (strength + sworddmgself < p -> hp || loc -> whoStart == -1 && side -> side == 1 || loc -> whoStart == 1 && side -> side == 0))
        {
            for( weapPos = 0; weapPos < 3; weapPos++)
                if( pWeaps[weapPos] != NULL)
                    if (pWeaps[weapPos] ->No == 1)
                        break;
            if ( weapPos == 3){
                weapPos = 0;
                return;
            }
            if ( pWeaps[weapPos] -> No == 1)
            {
                //detecting if enemy has enough damage to kill.
                if ( loc -> whoStart == -1 && side -> side == 1 || loc -> whoStart == 1 && side -> side == 0)
                {
                    int i = 0;
                    for( ; i < 3; i++)
                        if( p -> pWeaps[i] != NULL && p -> pWeaps[i] -> No == 0)
                        {
                            hasSword = 1; break;
                        }
                    
                    if( hasSword) sumDmg = p->strength + p -> pWeaps[i] -> damage;
                    else sumDmg = p -> strength;
                    
                } else {
                    int i = 0;
                    for( ; i < 3; i++)
                        if( p -> pWeaps[i] != NULL && p -> pWeaps[i] -> No == 0)
                        {
                            hasSword = 1; break;
                        }
                    if( hasSword) sumDmg = p->strength / 2 + p -> pWeaps[i] -> damage;
                    else sumDmg = p -> strength /2;
                    if (p -> type == 1) sumDmg = 0;
                    
                }

                if( sumDmg >= hp)//checking if sumdmg is enough
                {
                    //suicide bombing
                    hp = 0; p-> hp = 0;
                    mins = 38;
                    printT(); 
                    printf("%s %s %d used a bomb and killed %s %s %d\n", side -> col, name[type], no, p -> side -> col, name[p ->type], p -> no);    
                    loc -> endBattle();
                }
            }
        }
    }

    virtual void onWin( Warrior * p) //this won p.
    {
        if( p -> type == 3)
            hp += p -> preHp;
        if(side -> rewardWar())
        {
            hp += 8;
        }
        if( type == 0)
        morale += 0.2;
        loc -> DragRoarTest();
        //hq gaining elements
        loc -> warTake( this);
        int s = 1;
        if( side -> side == 0) s= -1;

        //loc -> DragonRoar( s);

        if( side -> side == 0)
            loc -> RaiseFlag(-1);
        else
            loc -> RaiseFlag(1);
        p -> side -> warriorDeath(p -> no); //died

        
    }
    virtual void preFight( Warrior * p) //checks bomb
    {
        enem_die = 0;
        //won = 0;
        preHp = hp;
        for( int i = 0; i < 3; i++)
        if( pWeaps[i] != NULL && pWeaps[i] -> No == 0)
            preSdmg = pWeaps[i] -> damage;

        useBomb(p);
        //shoot arrows:
        //
        
    }

    virtual void lionLoseLoy()
    {}

    virtual void moveForward( )
    {
        if( type == 2) 
        {
            steps++;
            if(steps == 2)
            {
                steps = 0;
                if (hp <= 9) hp = 1;
                else 
                hp -= 9; strength += 20;
            }
        }

        if (side -> side == 0)
        {
            loc = cities[++nloc];
            loc -> redWar = this;
            cities[nloc - 1]  -> redWar = NULL;

        }
        else if (side -> side == 1)
        {
            loc = cities[--nloc];
            loc -> blueWar = this;
            cities[nloc + 1] -> blueWar = NULL;

        }
        
    }
    void printMove()
    {        
        if( reachedHQ) return;
        printT();
        if (side -> side == 0)
            if (loc -> CityNo == N -1)
            {
                reachedHQ = 1;
                printf("%s %s %d reached blue headquarter with %d elements and force %d\n", side -> col, name[type], no, hp, strength);
                side -> victory();
                return;
            }
        if( side -> side == 1)
            if (loc -> CityNo == 0)
                {
                    reachedHQ = 1;
                    printf("%s %s %d reached red headquarter with %d elements and force %d\n", side -> col, name[type], no, hp, strength);
                    side -> victory();
                    return;
                }
        
        printf("%s %s %d marched to city %d with %d elements and force %d\n", side -> col, name[type], no, nloc, hp, strength);
    }
};
//sword bomb arrow 0, 1, 2

class ninja: public Warrior
{
    public:
    int weapPos = 0;
    ninja ( int a, int h, int warriorNo, HQ * side): Warrior( a, h, warriorNo, side)
    {
        type = 1;
        pWeaps[0] = getWeapon(warriorNo, this); 
        pWeaps[1] = getWeapon(warriorNo + 1, this); 
        if( pWeaps[0] -> damage == 0 && pWeaps[0] -> No == 0){ pWeaps[0] = NULL;
                    weapCnt --;}
        if( pWeaps[1] -> damage == 0 && pWeaps[1] -> No == 0){ pWeaps[1] = NULL;
        weapCnt --;}
    } 
    void counterA( Warrior * p){
        if( won)
        {
            won = 0;
            onWin (p);
            return;
        }
    }

};

class dragon: public Warrior
{
    public:
    dragon( int a, double h, int warriorNo, double hqH, HQ* side): Warrior( a, h, warriorNo, side)
    { 
        type = 0;
        if (h == 0) morale = 0;
        else
            morale = hqH / h;

        pWeaps[0] = getWeapon(warriorNo, this);
        if( pWeaps[0] -> damage == 0 && pWeaps[0] -> No == 0){ pWeaps[0] = NULL;
            weapCnt --;}
    }

};
class iceman: public Warrior
{
    public:
    iceman ( int a, int h, int warriorNo, HQ* side): Warrior( a, h, warriorNo, side)
    {
        type = 2;
        pWeaps[0] = getWeapon(warriorNo, this); 
        if( pWeaps[0] -> damage == 0 && pWeaps[0] -> No == 0){ pWeaps[0] = NULL;
            weapCnt --;}
    } 
};

class wolf: public Warrior
{
    int hasTypes[3] = {0};
    int weapPos = 0;
    public:
    wolf(  int a, int health, int no, HQ* side): Warrior( a, health, no, side)
    {
        type = 4;
    }

    void onWin( Warrior *p) //this won p
    {
        enem_die = 1;
        for (int i = 0; i < 3; i++)
        {
            if (weapCnt < 4)
                if ( p -> pWeaps[i] != NULL && hasTypes[p -> pWeaps[i] -> No] == 0)
                {
                    hasTypes[p -> pWeaps[i] -> No] = 1;

                    //giving weapon
                    p -> pWeaps[i] -> newOwner(this);
                    //finding next empty weapos
                    for( ; pWeaps[weapPos] != NULL; weapPos++)
                        if( weapPos == 3) weapPos = 0;

                    pWeaps[weapPos] = p -> pWeaps[i];

                    p -> pWeaps[i] = NULL;
                    weapCnt++;
                    p -> weapCnt --;
                }
        }
        if( p -> type == 3)
            hp += p -> preHp;
        if(side -> rewardWar())
        {
            hp += 8;
        }
        loc -> DragRoarTest();
        //hq gaining elements
        loc -> warTake( this);

        if( side -> side == 0)
            loc -> RaiseFlag(-1);
        else
            loc -> RaiseFlag(1);
        p -> side -> warriorDeath(p -> no); //died

    }
};

class lion: public Warrior
{
    public:
    int loyalty;
    lion ( int a, int h, int warriorNo, int hqH, HQ* side): Warrior( a, h, warriorNo, side), loyalty( hqH) {
        type = 3;
    }
    void lionLoseLoy()
    {
        loyalty -= K;
    }
    void runAway()
    {
        if (loyalty <= 0)
        {
            printT();
            printf( "%s lion %d ran away\n", side -> col, no);

            side -> warriorDeath(no);
        }
    
    }

};




int HQ :: warH[5] = {0};
int HQ :: warA[5] = {0};
char HQ :: color[2][7] = {"red", "blue"};
char HQ :: weaponName[3][7] = {"sword", "bomb", "arrow"};

int redl[5] = {2, 3, 4, 1, 0};
int bluel[5] = {3, 0, 1, 2, 4};

void HQ::init()
{
    nWarrior = 0;
    stop = 0;

}


Weapon:: Weapon (int i, Warrior * p): damage(i), pOwner(p) {}

void Sword:: updateDmg(int percent)
{
    damage = pOwner -> preSdmg * percent / 100;
}


Sword:: Sword(Warrior * p): Weapon( int(p -> strength * 2 / 10), p) {
        No = 0;
        
}

Bomb::Bomb(Warrior * p): Weapon( int( p -> strength * 4 / 10), p) {
        No = 1;}

void Bomb:: updateDmg () 
{
}

void Sword:: printWeap() {
    printf("sword(%d)", damage);
}

void Arrow:: printWeap() {
    printf("arrow(%d)", 3 - used);
}
void Bomb:: printWeap() {
    printf("bomb", damage);
}
Arrow:: Arrow( Warrior * p): Weapon( R, p) {
        No = 2;
}
void Arrow:: updateDmg () 
{
    damage = R;
}

void City:: arrowFight()
{
    Warrior * penem;
    if( CityNo < N-1 && redWar != NULL && cities[CityNo + 1] -> blueWar != NULL)
    {
        penem = cities[CityNo + 1] -> blueWar;
        redWar -> moved = 0;
        redWar -> shootArrow( penem);
    }
    if( CityNo > 1 && blueWar != NULL && cities[CityNo - 1] -> redWar != NULL )
    {
        penem = cities[CityNo - 1] -> redWar;
        blueWar -> moved = 0;
        blueWar -> shootArrow( penem);
    }
}

void City:: bombCheck()
{
    if (redWar != NULL && blueWar != NULL)
    {
        if( (CityNo % 2 == 1 && flag == 0) || flag == -1)
            whoStart = -1;
        else
            whoStart = 1;

        endFight = 0;
            redWar -> preFight(blueWar);
        if (redWar != NULL && blueWar != NULL)
            blueWar -> preFight(redWar);
    }
}

void City:: fight()
{
    mins = 40;
    int cnt = 0;
        //干架
    //finding if can shoot arrow.
    int sflag = 0; //shot flag 
    //if red side  

    if( redWar != NULL && blueWar != NULL)
    {
        if( (CityNo % 2 == 1 && flag == 0) || flag == -1)
        {
            redWar -> attack(blueWar);
            whoStart = -1;
            if(endFight) return;
            if( endFight == 0 && redWar != NULL && blueWar != NULL) blueWar -> counterA( redWar);
        }
        else
        {
            if ( redWar != NULL && blueWar != NULL)
            blueWar -> attack(redWar);
            whoStart = 1;
            if(endFight) return;
            if( endFight == 0 && redWar != NULL && blueWar != NULL) redWar -> counterA( blueWar);
        }
        if( redWar != NULL && blueWar != NULL && redWar -> won != 1 && blueWar -> won != 1)
            DragRoarTest();
    } return;
}

void City::DragRoarTest()
{
    if( blueWar != NULL && blueWar -> hp > 0 && redWar != NULL && redWar -> hp > 0)
    {
        RaiseFlag(0);
        DragonRoar(whoStart);
    }
    else if( blueWar != NULL && whoStart == 1 && blueWar -> hp > 0)
    {
        DragonRoar(whoStart);
    }
    else if( redWar != NULL && whoStart == -1 && redWar -> hp > 0)
    {
        DragonRoar(whoStart);
    }
    
}

void City :: warTake( Warrior * p)
{
    if( p != NULL)
    {
        printT();
        printf("%s %s %d earned %d elements for his headquarter\n", p -> side -> col, name[p -> type], p -> no, elements);
        hqTake( p -> side);
        return;
    }
    if( redWar != NULL && blueWar == NULL) 
    {
        printT();
        printf("red %s %d earned %d elements for his headquarter\n", name[redWar -> type], redWar -> no, elements);
        hqTake( redWar -> side);
    }
    else if( blueWar != NULL && redWar == NULL) 
    {
        printT();
        printf("blue %s %d earned %d elements for his headquarter\n", name[blueWar -> type], blueWar -> no, elements);
        hqTake( blueWar -> side);
    }
}

void HQ:: output(Warrior * p, int t, int end, int hasStopped)
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
        printf("%s %s %d born\n", col, name[t], nWarrior);
        if (t == 3) printf( "Its loyalty is %d\n", M);
        if (t == 0) cout << fixed << setprecision(2) << "Its morale is " << p -> morale << endl;
        //printf( "Its morale is %.2lf\n", p -> morale);
    }
}  

void City :: hqTake( HQ * hq)
{

    hq -> M += elements;
    elements = 0;
}

void City :: DragonRoar( int side = 0) //0 tests both, -1 only red, and 1 only blue
{

    if(( side == 0 || side == -1) && redWar -> type == 0 && redWar -> morale > 0.8)
    {
        
        printT();
        printf("red dragon %d yelled in city %d\n", redWar -> no, CityNo);
    }
    if(side == -1) return;
    if(blueWar -> type == 0 && blueWar -> morale > 0.8)
    {
        printT();
        printf("blue dragon %d yelled in city %d\n", blueWar -> no, CityNo);
    }
}

void City :: endBattle()
{
    if (endFight) 
    {
        endFight = 0;
        return;
    };
    endFight = 1;
    mins = 40;

    
    if (blueWar -> hp <= 0 && redWar -> hp <= 0)
    {
        //printf("both red %s %d and blue %s %d died in city %d\n", name[redWar -> type], redWar -> no, name[blueWar -> type], blueWar -> no, CityNo);
        blueWar -> side -> warriorDeath( blueWar -> no);
        redWar -> side -> warriorDeath( redWar -> no);
        return;
    }
    else if (blueWar -> hp > 0 && redWar -> hp > 0)
    {
        printT();
        printf("both red %s %d and blue %s %d were alive in city %d\n", name[redWar -> type], redWar -> no, name[blueWar -> type], blueWar -> no, CityNo);
        if( blueWar -> type == 0)
            blueWar -> morale -= 0.2;
        if( redWar -> type == 0)
            redWar -> morale -= 0.2;
        
        DragonRoar(whoStart);
    }
    else if (redWar -> hp <= 0)
    {
        printT();
        printf("red %s %d was killed in city %d\n", name[redWar -> type], redWar -> no, CityNo);
        blueWar -> won = 1;



    }
    else if (blueWar -> hp <= 0)
    {
        printT();
        printf("blue %s %d was killed in city %d\n", name[blueWar -> type], blueWar -> no, CityNo);
        redWar -> won = 1;

        //Blue Wins
        //blueWar -> side -> warriorDeath(blueWar -> no);
    }
}

void City :: printFlag( int i)
{
    if( i == -1) 
    {   printT();
    printf("red flag raised in city %d\n", CityNo);
    }
    if( i == 1) 
    {
        printT();
        printf("blue flag raised in city %d\n", CityNo);
    }

}

void City :: RaiseFlag(int i) //-1 red, 1 blue, 0 even.
{
    if( flag != 0)
    {
        if( last_won == i && flag != i && i != 0)
        {
            printFlag(i);
            flag = i;
        }
    }
    else if( last_won == i)
    {
        if( flag != i)
        { 
            printFlag(i);
            flag = i; 
        }
    }

    //if( i != 0 || flag == 0)
    last_won = i;
}


bool HQ:: makeWarrior(int t)
{
    
    if (M < warH[t]) 
    {
        return false;
    }

    M -= warH[t]; //making warrior and subtracting health
    // what warrior to make. //dragon, ninja, iceman, lion, wolf.
    nWarrior++;
    switch (t)
    {
        
        case 0: 
        {
            warriors[nWarrior] = new dragon (warA[t], warH[t], nWarrior, M, this);
        }; break;
        case 1: {
            warriors[nWarrior] = new ninja (warA[t], warH[t], nWarrior, this);
        }; break;
        case 2: {
            warriors[nWarrior] = new iceman (warA[t], warH[t], nWarrior, this);
        }; break;
        case 3: {
            warriors[nWarrior] = new lion (warA[t], warH[t], nWarrior, M, this);
        }; break;
        case 4:{
            warriors[nWarrior] = new wolf (warA[t], warH[t], nWarrior, this); break;
        }
    }
    output(warriors[nWarrior], t);
    return true;
    //warrior_Alive[nWarrior] = 1;  
    
}

void HQ :: warriorDeath( int warNo)
{
    if (side == 0)
    {
        warriors[warNo] -> loc -> redWar = NULL;
    }
    else 
        warriors[warNo] -> loc -> blueWar = NULL;
    delete warriors[warNo];
    warriors[warNo] = NULL;
}


int main()
{
    int n;   
    cin >> n;
    for (int i = 1; i <= n; i++)
    {
        minCnt = 0; 
        hour = 0;
        mins = 0;
        minCnt = 0;
        warEndr = 0;
        warEndb = 0;
        int endFlag = 0;

        int M;

        cin >> M >> N >> R >> K >> T;
        
        N += 2;

        for (int i = 0; i <= N; i++)
            cities[i] = new City(i);

        HQ * pBase[2];
        pBase[0] = new HQ(M, 0);
        pBase[1] = new HQ(M, 1);

        for (int i = 0; i < 5; i++)
            cin >> pBase[0] -> warH[i];
        
        //dragon, ninja, iceman, lion, wolf.
        for (int i = 0; i < 5; i++)
            cin >> pBase[0] -> warA[i];

        cout << "Case " << i << ':'<< endl;
        int r = 0;
        int b = 0;
        int rcnt = 0;
        int bcnt = 0;
        
        while (minCnt <= T)
        {
            mins = 0;    

            if(r >= 5) r -= 5;
            if(b >= 5) b -= 5;
            
            if (pBase[0] -> makeWarrior(redl[r]))
                r++;
            if( pBase[1] -> makeWarrior(bluel[b]))
                b++;

            
            if( minCnt + 5 > T) break;

            mins = 5;
            for( int i = 0; i < 2; i++)
                for( int j = 1; j <= pBase[i] -> nWarrior; j++)
                {
                    if(pBase[i] -> warriors[j] != NULL)
                        if( pBase[i] -> warriors[j] -> type == 3)
                        {
                            pBase[i] -> warriors[j] -> runAway();
                        }
                }

            if( minCnt + 10 > T) break;
            
            mins = 10;

            for( int i = N - 1; i >= 0; i-- )
            {
                if(!cities[i]) continue;
                if( cities[i] -> redWar != NULL)
                    cities[i] -> redWar -> moveForward();
                
            }

            for( int i = 1; i < N; i++)
            {
                if( cities[i] -> blueWar != NULL)
                    cities[i] -> blueWar -> moveForward();
            }
            //cout move
            for( int i = 0; i < N; i++)
            {
                if( cities[i] -> redWar != NULL)
                    cities[i] -> redWar -> printMove();
                if( cities[i] -> blueWar != NULL)
                    cities[i] -> blueWar -> printMove();
            }

            if(warEndr == 2 || warEndb == 2) break;
            if( minCnt + 30 > T) break;

            mins = 30;
            for( int i = 1; i < N; i++)
                cities[i] -> gainElements();

            for( int i = 1; i < N -1; i++)
                cities[i] -> warTake();

            if( minCnt + 35 > T) break;
            
            mins = 35;
            for( int j = 1; j < N - 1; j++)
                if( cities[j] ->blueWar != NULL || cities[j] -> redWar != NULL)
                    cities[j] -> arrowFight();

            //cleaning up dead bodies
            for( int j = 1; j < N - 1; j++)
                if( cities[j] ->blueWar != NULL || cities[j] -> redWar != NULL)
                {
                    if( cities[j] -> blueWar != NULL && cities[j] -> blueWar -> hp <= 0 && (cities[j] -> redWar == NULL || (cities[j] -> redWar != NULL && cities[j] -> redWar -> hp <= 0)))
                        pBase[1] -> warriorDeath( cities[j] -> blueWar -> no);
                    if( cities[j] ->redWar != NULL && cities[j] ->redWar -> hp <= 0 && (cities[j] -> blueWar == NULL|| (cities[j] -> blueWar != NULL && cities[j] -> blueWar -> hp <= 0) ))
                        pBase[0] -> warriorDeath ( cities[j] -> redWar -> no);
                }
            

            if( minCnt + 38 > T) break;
            mins = 38;
            for( int j = 1; j < N - 1; j++)
                if( cities[j] ->blueWar != NULL || cities[j] -> redWar != NULL)
                    cities[j] -> bombCheck();



            if( minCnt + 40 > T) break;
            mins = 40;
            for( int j = 1; j < N - 1; j++)
                if( cities[j] ->blueWar != NULL || cities[j] -> redWar != NULL)
                    cities[j] -> fight();



            if( minCnt + 50 > T) break;

            mins = 50;
            for( int i = 0; i < 2; i++)
                pBase[i] -> printHealth();
            
            if( minCnt + 55 > T) break;
            
            mins = 55;

            for( int i = 0; i <= N; i++)
                if( cities[i] -> redWar != NULL)
                cities[i] -> redWar -> reportWeaps();

            for( int i = 0; i < N; i++)
                if( cities[i] -> blueWar != NULL)
                cities[i] -> blueWar -> reportWeaps();
            
            hour++;
            minCnt += 60;
        }
        
    }
    system("pause");
    return 0;
}