//花了5个小时 实在研究不出来，用pdl做出来的。。。
#include<iostream>
#include<cstring>
#include<cstdlib>
#include<set>

int N;
int list1[20][20];
int dist[20];
int _result;
int _best__result;

void _input() {
 std::cin >> N;
 for (int _tmp0 = 1; _tmp0 <= N; _tmp0++)
  for (int _tmp1 = 1; _tmp1 <= N; _tmp1++)
   std::cin >> list1[_tmp0 - 1][_tmp1 - 1];
}

void _output() {
 std::cout << _best__result << std::endl;
}

std::set<int> _set0;

void _update() {
 if (_result >= _best__result)
  return;
 _best__result = _result;
}

void _find_dist(int _step, int _sum1) {
 if (_step == N - 1 + 1) {
  _result = _sum1;
  _update();
  return;
 }
 int __sum1 = _sum1;
 for (dist[_step] = 1; dist[_step] <= 20; dist[_step]++) {
  int _tmp0 = dist[_step];
  if (_set0.find(_tmp0) != _set0.end())
   continue;
  _sum1 = __sum1;
  if (_step - 1 >= 0) {
   int _tmp1 = dist[_step - 1];
   if (_tmp1 > N)
    continue;
   int _tmp2 = dist[_step + 1 - 1];
   if (_tmp2 > N)
    continue;
   _sum1 += list1[_tmp1 - 1][_tmp2 - 1];
  }
  if (!(_sum1 < _best__result))
   continue;
  _set0.insert(_tmp0);
  _find_dist(_step + 1, _sum1);
  _set0.erase(_tmp0);
 }
}

void _solve() {
 _best__result = 19001;
 _find_dist(0, 0);
}

int main() {
 _input();
 _solve();
 _output();
 return 0;
}