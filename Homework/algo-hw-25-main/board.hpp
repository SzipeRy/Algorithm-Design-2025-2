#ifndef __BOARD_H_INCLUDED__

#define __BOARD_H_INCLUDED__

#include <utility>
#include <vector>

class board_t {
public:
	std::vector<std::vector<int>> b;

	void play_move(char c);
	bool submit(std::vector<std::vector<int>> e);
	void print_board();
	void print_stats();

	board_t(std::vector<std::vector<int>> _b);
private:
	int n, move_count;
	std::pair<int, int> space;

	inline bool valid(std::pair<int, int> p);
	bool check_board();
};

#endif
