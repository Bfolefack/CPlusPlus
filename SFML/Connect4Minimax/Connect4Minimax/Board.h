#pragma once
#include <bitset>
#include <iostream>
#include <unordered_map>
#include <vector>

static class Board
{
public:

	//unsigned long long x_board;
	//unsigned long long o_board;
	//int turn_counter;
	//std::vector<int> moves;
	static std::unordered_map<unsigned long long, int> scores;

	//std::vector<int> heights = {0, 0, 0, 0, 0, 0, 0};

	static std::array<unsigned long long, 5> new_board();
	static unsigned int turn_count(const std::array<unsigned long long, 5>& board);
	static void print_board(const std::array<unsigned long long, 5>& board);
	static std::array<unsigned long long, 5> move(const std::array<unsigned long long, 5>& board, const unsigned long long column);
	static std::array<std::array<unsigned long long, 5>, 7> get_children(std::array<unsigned long long, 5> board);
	static bool evaluate_board(unsigned long long b);
	static int evaluate_board(std::array<unsigned long long, 5> board);
	static std::pair<std::array<unsigned long long, 5>, int> minimax(std::array<unsigned long long, 5> board, int depth,
	                                                                 bool max_player, int max_depth);
};

