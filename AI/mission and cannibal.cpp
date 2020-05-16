#include <iostream>
#include <cstring>
#include <queue>

using namespace std;
const int MAX = 1000;
int explored[MAX][MAX][2] = {0}; //left goal is [m][m][1] = 1
int m, n;
int failed = 0;
int steps = -1;


struct node
{
    int m;
    int c;
    int b; // 0 = right, 1 left
    int d; //1 left, -1 right
    int cnt;
};

void bfs()
{
    queue <node> q;
    node start;
    start.m = 0; start.c = 0; 
    start.b = 0; 
    start.cnt = 0; 
    start.d = 1;
    explored[0][0][0] = 1;
    q.push(start);
    while (!q.empty())
    {
        node parent = q.front();
        q.pop();
        for (int i = 0; i <= n; i++)//take all actions i for missionary, j for cannibals
            for(int j = 0; j <= n - i; j++)
            {
                if (i == 0 && j == 0) continue;
                if (j > i && i != 0) break;
                node child;
                child.m = parent.m + i * parent.d;
                child.c = parent.c + j * parent.d;
                if (child.m < 0 || child.c < 0) continue;
                if (child.m > m || child.c > m) continue; 
                if (child.m != child.c && child.m != 0 && child.m != m) continue;
                //if child.d = 1 is left, than boat is on the left.
                if (parent.b == 1)
                {
                    child.b = 0;
                }
                else {child.b = 1;}

                child.d = parent.d * -1;
                
                if (explored[child.m][child.c][child.b]) continue;
                explored[child.m][child.c][child.b] = 1;
                child.cnt = parent.cnt + 1;
                if (explored[m][m][1]) {steps = child.cnt; return;}
                q.push(child);
            }
    }   
    failed = 1;
}

/* bfs (problem) returns a solution or a failure
{
    node with state, initial 
    if problem.test(node.state) return solution(node)
    frontiner 
    explored
    loop do
        if empty?(frontier) return failure
        node pop(frontiner)
        add node to explored.
        for each action in actions(node.state) do
        child child node
        if child is not in explored or frontier
            if test(child state) return solution
            frontier insert (child,frontier )
}
*/

int main()
{
    cin >> m >> n;
    bfs();
    if (failed) cout << -1;
    else cout << steps;
    system("pause");
}