#include "board.hpp"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

inline bool board_t::valid(std::pair<int, int> p) {
	return 0 <= p.first && p.first < n && 0 <= p.second && p.second < n;
}

void board_t::play_move(char c) {
	std::pair<int, int> dir;
	switch(c) {
		case 'U':
			dir = std::make_pair(1, 0);
			break;
		case 'D':
			dir = std::make_pair(-1, 0);
			break;
		case 'L':
			dir = std::make_pair(0, 1);
			break;
		case 'R':
			dir = std::make_pair(0, -1);
			break;
		default:
			std::cerr << "Invalid Move" << std::endl;
			exit(1);
	}
	std::pair<int, int> swapping = std::make_pair(dir.first + space.first, dir.second + space.second);
	if(!valid(swapping)) {
		std::cerr << "Illegal Move" << std::endl;
		exit(1);
	}
	std::swap(b[space.first][space.second], b[swapping.first][swapping.second]);
	std::swap(space, swapping);
	move_count++;
}

bool board_t::submit(std::vector<std::vector<int>> e) {
	bool ok = true;
	int s = (int) e.size();
	for(int _=0, i=(n - s)/2; _<s; ++_, ++i) {
		for(int __=0, j=(n - s)/2; __<s; ++__, ++j) {
			int board_color = b[i][j];
			int expected_color = e[_][__];
			ok &= (board_color == expected_color);
		}
	}
	return ok;
}

void board_t::print_board() {
	std::cout << "-----------" << std::endl;
	for(int i=0; i<n; ++i) {
		for(int j=0; j<n; ++j) {
			std::cout << (b[i][j] != -1 ? std::to_string(b[i][j]) : " ") << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << "-----------" << std::endl;
}

void board_t::print_stats() {
	std::cout << "> Move Uses: " << move_count << std::endl;
}

board_t::board_t(std::vector<std::vector<int>> _b) {
	b = _b;
	n = (int) b.size();
	move_count = 0;

	for(int i=0; i<n; ++i) {
		for(int j=0; j<n; ++j) {
			if(b[i][j] == -1) {
				space = std::make_pair(i, j);
			}
		}
	}
}
