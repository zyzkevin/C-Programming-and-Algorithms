#include<iostream>
#include<cstring>
#include<cstdlib>
#include<set>

int n;
int dis[100][100];
int point[100];
int _result;
int _best__result;

void _input() {
	std::cin >> n;
	for (int _tmp0 = 1; _tmp0 <= n; _tmp0++)
		for (int _tmp1 = 1; _tmp1 <= n; _tmp1++)
			std::cin >> dis[_tmp0 - 1][_tmp1 - 1];
}

void _output() {
	std::cout << _best__result << std::endl;
}

void _update() {
	if (_result >= _best__result)
		return;
	_best__result = _result;
}

void _find_point(int _step, int _sum1) {
	if (_step == n - 1 + 1) {
		_result = _sum1;
		if (!(point[1 - 1] == 1))
			return;
		if (!(point[n - 1] == n))
			return;
		_update();
		return;
	}
	int __sum1 = _sum1;
	for (point[_step] = 1; point[_step] <= 10000; point[_step]++) {
		_sum1 = __sum1;
		if (_step - 1 >= 0) {
			int _tmp0 = point[_step - 1];
			if (_tmp0 > n)
				continue;
			int _tmp1 = point[_step + 1 - 1];
			if (_tmp1 > n)
				continue;
			_sum1 += dis[_tmp0 - 1][_tmp1 - 1];
		}
		if (!(_sum1 < _best__result))
			continue;
		_find_point(_step + 1, _sum1);
	}
}

void _solve() {
	_best__result = 9999000001;
	_find_point(0, 0);
}

int main() {
	_input();
	_solve();
	_output();
	system("pause");
	return 0;
}