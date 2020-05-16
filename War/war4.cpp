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
int warEnd = 0;


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
    int No;
    int damage;
    virtual ~Weapon() {}
    virtual int getDmg() // returns damage
    {
        return damage;
    }
    virtual void useWeapon () {}
    virtual void updateDmg () {}
    
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
    void useWeapon();
};

class Arrow: public Weapon
{
    public:
    Arrow( Warrior * p);
    void updateDmg (int percent = 100);


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
    int whoStart = 0;

    City(int i): CityNo(i){}

    int flag = 0; //-1 red, 0 none, 1 blue.
    int last_won = 0; //-1 red, 1 blue.

    void endBattle();
    void fight();
    void deleteR(int warNo);
    void deleteB(int warNo);
    void preFight();
    void DragonRoar( int side);
    int endFight = 1;
};

City * cities[20];


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
    int side;

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
    void makeWarrior(int t);

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
        printT();
        if( side) //blue hq
            printf("%s headquarter was taken\n", color[0]);
        else
            printf("%s headquarter was taken\n", color[1]);
        warEnd = 1;
    }
    void printHealth()
    {
        printT();
            printf("%d elements in %s headquarter\n", M, col);
    }
    

    void output(Warrior * p, int t, int end = 0, int hasStopped = 0)
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
        }
    }  
};

class Warrior
{
    friend class City;
    protected:
    City* loc;

    
    public:
    float morale;
    int nloc;
    int strength;
    int preHp;
    int no;
    int preSdmg;
    HQ* side;
    int weapPos = 0;
    int type;
    int weapCnt = 0;
    int enem_die = 0;
    int hp;
    int steps = 0;
    int won = 0;

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
        int scnt =0; int bcnt =0; int acnt = 0;
        for( int i = 0; i < 10; i++)
            if( pWeaps[i] != NULL)
                switch (pWeaps[i] -> No)
                {
                    case 0: scnt++; break;
                    case 1: bcnt++; break;
                    case 2: acnt++; break;
                }
        printT();
        printf("%s %s %d has %d sword %d bomb %d arrow and %d elements\n", side -> col, name[type], no, scnt, bcnt, acnt, hp);

    }

    virtual void runAway() {}
    friend class HQ;

    virtual void hurt (int n){
        hp -= n;
        
        if (hp <= 0)
        {
            hp = 0;
            loc -> endBattle(); 
        }
    };

    virtual void counterA( Warrior * p)
    {
        p ->hurt(pWeaps[0] -> getDmg() / 2);
    }

    virtual void attack(Warrior * p)
    {    
        //        if(enem_die)
        //    return;
        
        
        if( p == NULL || p -> hp == 0)
        {
            loc -> endBattle();
            enem_die = 1;
            return;
        } 
        

        //if is bomb hurting self
        if( pWeaps[0] -> No == 1)
        {
            p -> hp -= pWeaps[0] -> getDmg();
            hp -= pWeaps[0] -> getDmg() / 2;
            //killed itself with bomb
            delete pWeaps[0];
            pWeaps[0] = NULL;
            weapCnt --;
            if( hp <= 0 && p -> hp <= 0)
            {
                hp = 0; p-> hp = 0;
                loc -> endBattle();
                return;
            }
            else if(hp <= 0)
            {
                hp = 0;
                loc -> endBattle();
                p -> onWin(this);
                return;
            }
            else if( p -> hp <= 0)
            {
                p-> hp = 0;
                loc -> endBattle();
                if( won)
                    onWin (p);
                return;
            }
            //weapPos++;
        }
        else if( pWeaps[0] -> No == 2 && pWeaps[0] -> used == 2)
        {
            p -> hurt( pWeaps[0] -> getDmg());
            delete pWeaps[0];
            pWeaps[0] = NULL;
            weapCnt --;
            //weapPos++;
        }
        else 
            p -> hurt( pWeaps[0] -> getDmg());


        if( pWeaps[0] != NULL)
            pWeaps[0] -> useWeapon();

        if( won)
            onWin (p);
        else
            lionLoseLoy();

        /*
        redWar -> side -> warriorDeath(redWar -> no);
        blueWar -> side -> warriorDeath(blueWar -> no);
        */
    }

    //
    virtual void onWin( Warrior * p) //this won p.
    {
        if( p -> type == 3)
            hp += p -> preHp;
        p -> side -> warriorDeath(p -> no); //died
        if(side -> rewardWar())
            hp += 8;
        
    }
    virtual void preFight( Warrior * p)
    {
        //check if both sides have weapon..
        enem_die = 0;
        won = 0;
        preHp = hp;
        if( pWeaps[0] -> No == 0)
        preSdmg = pWeaps[0] -> damage;

        if ((weapCnt == 0 && p-> weapCnt == 0 ))
        {
            loc -> endBattle();
            return;
            //end battle; return;
        }
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
        printT();
        if (side -> side == 0)
            if (loc -> CityNo == N -1)
            {
                printf("%s %s %d reached blue headquarter with %d elements and force %d\n", side -> col, name[type], no, hp, strength);
                side -> victory();
                return;
            }
        if( side -> side == 1)
            if (loc -> CityNo == 0)
                {    
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
    } 
    void counterA(){}


    void attack(Warrior * p)
    {    
        //        if(enem_die)
        //    return;
        if( weapPos == 2) weapPos = 0;

        
        //find first weapon
        int cnt = 0;
        for( ; pWeaps[weapPos] == NULL; weapPos++ )
        {
            if( weapPos == 10) weapPos = 0;
            cnt++;
            if( pWeaps[weapPos] != NULL) break;
            if( cnt >= 12 || weapCnt <= 0) return;
        }
        
        
        if( p == NULL || p -> hp == 0)
        {
            loc -> endBattle();
            enem_die = 1;
            return;
        } 

        //if is bomb hurting self
        if( pWeaps[weapPos] -> No == 1)
        {
            p -> hp -= pWeaps[weapPos] -> getDmg();
            //killed itself with bomb
            delete pWeaps[weapPos];
            pWeaps[weapPos] = NULL;
            weapCnt --;
            if( hp <= 0 && p -> hp <= 0)
            {
                hp = 0; p-> hp = 0;
                loc -> endBattle();
                return;
            }
            else if(hp <= 0)
            {
                hp = 0;
                loc -> endBattle();
                p -> onWin(this);
                return;
            }
            else if( p -> hp <= 0)
            {
                p-> hp = 0;
                loc -> endBattle();
                if( won)
                    onWin (p);
                return;
            }
            //weapPos++;
        }
        else if( pWeaps[weapPos] -> No == 2 && pWeaps[weapPos] -> used)
        {
            p -> hurt( pWeaps[weapPos] -> getDmg());
            delete pWeaps[weapPos];
            pWeaps[weapPos] = NULL;
            weapCnt --;
            //weapPos++;
        }
        else 
            p -> hurt( pWeaps[weapPos] -> getDmg());


        if( pWeaps[weapPos] != NULL)
            pWeaps[weapPos] -> useWeapon();
        weapPos ++;

        if( won)
            onWin (p);

        /*
        redWar -> side -> warriorDeath(redWar -> no);
        blueWar -> side -> warriorDeath(blueWar -> no);
        */
    }

};

class dragon: public Warrior
{
    public:
    dragon( int a, int h, int warriorNo, int hqH, HQ* side): Warrior( a, h, warriorNo, side)
    { 
        type = 0;
        if (h == 0) morale = 0;
        else
        morale = hqH / h;

        pWeaps[0] = getWeapon(warriorNo, this); 
    }

};
class iceman: public Warrior
{
    public:
    iceman ( int a, int h, int warriorNo, HQ* side): Warrior( a, h, warriorNo, side)
    {
        type = 2;
        pWeaps[0] = getWeapon(warriorNo, this); 
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

    void preFight( Warrior * p){
        won = 0;
        enem_die = 0;
        preHp = hp;
        //if not wolf, stealing weapon
        int stolencnt = 0;


    }

    void onWin( Warrior *p) //this won p
    {
        enem_die = 1;
        for (int i = 0; i < 3; i++)
        {
            if (weapCnt < 10)
                if ( p -> pWeaps[i] != NULL && hasTypes[p -> pWeaps[i] -> No] == 0)
                {
                    hasTypes[p -> pWeaps[i] -> No] = 1;

                    //giving weapon
                    p -> pWeaps[i] -> newOwner(this);
                    //finding next empty weapos
                    for( ; pWeaps[0] != NULL; weapPos++)
                        if( weapPos == 10) weapPos = 0;

                    pWeaps[weapPos] = p -> pWeaps[i];

                    p -> pWeaps[i] = NULL;
                    weapCnt++;
                    p -> weapCnt --;
                }
        }
        if( p -> type == 3)
            hp += p -> preHp;
        p -> side -> warriorDeath(p -> no); //died
        if(side -> rewardWar())
            hp += 8;
    }
};

class lion: public Warrior
{
    public:
    int loyalty;
    lion ( int a, int h, int warriorNo, int hqH, HQ* side): Warrior( a, h, warriorNo, side), loyalty( hqH) {
        type = 3;
        pWeaps[0] = getWeapon(warriorNo, this); 
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

void Sword:: updateDmg(int percent = 100)
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

Arrow:: Arrow( Warrior * p): Weapon( R, p) {
        No = 2;
}
void Arrow:: updateDmg (int percent = 100) 
{
    damage = R;
}

void City:: preFight()
{
    if (redWar != NULL && blueWar != NULL)
    {
        redWar -> preFight(blueWar);
        blueWar -> preFight(redWar);
        endFight = 0;
    }
}

void City:: fight()
{
        mins = 40;
        int cnt = 0;
            //干架
        if( (CityNo % 2 == 1 && flag == 0) || flag == -1)
        {
            redWar -> attack(blueWar);
            whoStart = -1;
            if( blueWar != NULL) blueWar -> counterA( redWar);
        }
        else
        {
            blueWar -> attack(redWar);
            whoStart = 1;
            if( redWar != NULL) redWar -> counterA( blueWar);
        }
        if( redWar == NULL  || blueWar == NULL)
            {
                endBattle();
                return;
            }
        if( endFight == 0)
            if( blueWar -> weapCnt == 0 && redWar -> weapCnt == 0) 
                endBattle();
        cnt ++;
        if( cnt >= 30)
        {
            int dmg_sum = 0; int dmg_sump = 0;
            for( int i = 0; i < 10; i++)
            {
                if (redWar -> pWeaps[i] != NULL) dmg_sum += redWar -> pWeaps[i] -> damage;
                if (blueWar -> pWeaps[i] != NULL) dmg_sump += blueWar -> pWeaps[i] -> damage;
            }
            
            if( endFight == 0)
                if( dmg_sump == 0 && dmg_sum == 0) 
                    endBattle();
        }
    return;
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
    if (endFight) return;
    endFight = 1;
    mins = 40;
    printT();

    
    if (blueWar -> hp <= 0 && redWar -> hp <= 0)
    {
        deathCnt ++;
        if( deathCnt == 1)
        {
            printf("both red %s %d and blue %s %d died in city %d\n", name[redWar -> type], redWar -> no, name[blueWar -> type], blueWar -> no, CityNo);
            blueWar -> side -> warriorDeath( blueWar -> no);
            redWar -> side -> warriorDeath( redWar -> no);
        }
        return;
    }
    else if (blueWar -> hp > 0 && redWar -> hp > 0)
    {
        printf("both red %s %d and blue %s %d were alive in city %d\n", name[redWar -> type], redWar -> no, name[blueWar -> type], blueWar -> no, CityNo);
        if( blueWar -> type == 0)
            blueWar -> morale -= 0.2;
        if( redWar -> type == 0)
            redWar -> morale -= 0.2;
        DragonRoar(whoStart);
    }
    else if (redWar -> hp <= 0)
    {
        printf("blue %s %d killed red %s %d in city %d remaining %d elements\n", name[blueWar -> type], blueWar -> no, name[redWar -> type], redWar -> no, CityNo, blueWar -> hp);
        blueWar -> won = 1;
        if( last_won == 1)
            flag = 1;  
        else
            flag = 0;
        last_won = 1;
        if( blueWar -> type == 0)
            blueWar -> morale += 0.2;
        DragonRoar(whoStart);

    }
    else if (blueWar -> hp <= 0)
    {
        printf("red %s %d killed blue %s %d in city %d remaining %d elements\n", name[redWar -> type], redWar -> no, name[blueWar -> type], blueWar -> no, CityNo, redWar -> hp);
        redWar -> won = 1;
        DragonRoar(-1);
        if( last_won == -1)
            flag = -1;  
        else
            flag = 0;
        last_won = -1;
                
        if( redWar -> type == 0)
            redWar -> morale += 0.2;
        DragonRoar(whoStart);
        //Blue Wins
        //blueWar -> side -> warriorDeath(blueWar -> no);
    }
}

void HQ:: makeWarrior(int t)
{
    
    if (M <= warH[t]) 
    {
        stop = 1;
        return;
    }
    if(stop) return;

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
        warEnd = 0;

        int M;

        cin >> M >> N >> K >> T;
        
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
            
            pBase[0] -> makeWarrior(redl[r]);
            pBase[1] -> makeWarrior(bluel[b]);

            
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

            if(warEnd) break;


            if( minCnt + 35 > T) break;
            
            mins = 35;
            for( int j = 0; j < N; j++)
                if( cities[j] ->blueWar != NULL || cities[j] -> redWar != NULL)
                    cities[j] -> preFight();

            for( int j = 0; j < N; j++)
                if( cities[j] ->blueWar != NULL || cities[j] -> redWar != NULL)
                    cities[j] -> fight();



            if( minCnt + 50 > T) break;

            mins = 50;
            for( int i = 0; i < 2; i++)
                pBase[i] -> printHealth();
            
            if( minCnt + 55 > T) break;
            
            mins = 55;

            for( int i = 0; i < N; i++)
            {
                if( cities[i] -> redWar != NULL)
                cities[i] -> redWar -> reportWeaps();
                if( cities[i] -> blueWar != NULL)
                cities[i] -> blueWar -> reportWeaps();
            }
            
            r++;
            b++;
            hour++;
            minCnt += 60;
        }
        
    }
    system("pause");
    return 0;
}