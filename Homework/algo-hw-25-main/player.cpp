#include <iostream>
#include <string>

#include "board.hpp"

int main(int argc, char *argv[]) {
	int n;
	std::cin >> n;
	std::vector<std::vector<int>> b(n, std::vector<int> (n));
	for(int i=0; i<n; ++i) {
		for(int j=0; j<n; ++j) {
			std::cin >> b[i][j];
		}
	}
	int m = n - 2;
	std::vector<std::vector<int>> e(m, std::vector<int> (m));
	for(int i=0; i<m; ++i) {
		for(int j=0; j<m; ++j) {
			std::cin >> e[i][j];
		}
	}
	std::string moves;
	std::cin >> moves;

	board_t board(b);
	for(auto c: moves) {
		if(c != 'S') {
			board.play_move(c);
		}
	}
	bool correct = board.submit(e);
	if(correct) {
		std::cout << "CORRECT" << std::endl;
		board.print_stats();
	}
	else {
		std::cout << "WRONG" << std::endl;
	}
}
