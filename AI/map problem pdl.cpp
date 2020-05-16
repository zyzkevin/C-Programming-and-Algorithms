#include<iostream>
#include<cstring>
#include<cstdlib>
#include<set>

int m;
int n[10000];
int start;
int end;
int _out0;

void _input() {
	std::cin >> m;
	for (int _tmp0 = 1; _tmp0 <= m; _tmp0++)
		std::cin >> n[_tmp0 - 1];
}

void _output() {
	std::cout << _out0 << std::endl;
	exit(0);
}

void _solve() {
	for (start = 1; start <= m; start++) {
		_out0 = n[start - 1];
		for (end = 1; end <= m; end++) {
			if (!(n[start - 1] == n[end - 1]))
				continue;
			if (!(start != end))
				continue;
			_output();
		}
	}
}

int main() {
	_input();
	_solve();
	std::cout << "No Solution!" << std::endl;
    system("pause");
	return 0;
}
