#include "Board.h"

#include <array>
#include <chrono>
#include <utility>
int counter = 0;
long long child_time = 0;
long long eval_time = 0;
long long move_time = 0;
long long vector_time = 0;

std::unordered_map<unsigned long long, int> Board::scores{};

//Board::Board()
//{
//	turn_counter = 0;
//	x_board = 0uL;
//	o_board = 0uL;
//}

std::array<unsigned long long, 5> Board::new_board()
{
	//x_board, o_board, move_history pt. 1, ,move_history pt. 2, turn_count & heights
	return { 0, 0, 0, 0, 0};
}

unsigned Board::turn_count(const std::array<unsigned long long, 5>& board)
{
	return static_cast<unsigned int>(board[4] >> (8 * 7));
}

//Board::Board(unsigned long long X, unsigned long long O, int turn, std::vector<int> h, std::vector<int> mvs)
//{
//	x_board = X;
//	o_board = O;
//	turn_counter = turn;
//	heights = std::move(h);
//	moves = std::move(mvs);
//	//std::cout << std::bitset<64>(hash) << std::endl;
//}

void Board::print_board(const std::array<unsigned long long, 5>& board)
{
	std::bitset<64> Xbit(board[0]);
	std::bitset<64> Obit(board[1]);

	for (int i = 6; i >= 0; i--)
	{
		std::cout << " ";
		for (int j = 0; j < 7; j++)
		{
			//std::cout << 7 * j + i << (7 * j + i < 10 ? "  " :" ");
			if (Xbit[7 * j + i])
				std::cout << "X ";
			else if (Obit[7 * j + i])
				std::cout << "O ";
			else
				std::cout << ". ";
		}
		std::cout << std::endl;
	}
	int count = 0;
	//for(const auto i : moves)
	//{
	//	count++;
	//	std::cout << " " << (count  % 2 == 0 ? "O" : "X") << i << " ";
	//}

	std::cout << std::bitset<64> (board[4]) << std::endl;
	std::cout << std::endl;

	std::cout << "Child Time: " << child_time << std::endl;
	std::cout << "Eval Time: " << eval_time << std::endl;
	std::cout << "Move Time:" << move_time << std::endl;
	std::cout << "Vector Time: " << vector_time << std::endl;
	std::cout << "Move Count:" << counter << std::endl;
}

std::array<unsigned long long, 5> Board::move(const std::array<unsigned long long, 5>& board, const unsigned long long column)
{
	//6 13 20 27 34 41 48
	//5 12 19 26 33 40 47
	//4 11 18 25 32 39 46
	//3 10 17 24 31 38 45
	//2  9 16 23 30 37 44
	//1  8 15 22 29 36 43
	//0  7 14 21 28 35 42

	//x_board, o_board, move_history pt. 1, ,move_history pt. 2, turn_count & heights

	auto start = std::chrono::high_resolution_clock::now();


	std::array<unsigned long long, 5> new_board = board;
	const int column_height = (new_board[4] >> ((column - 1) * 3)) & 0b111;
	const unsigned int shift = column_height + ((column - 1) * 7);
	const auto move = 1ull << shift;

	//Make move
	if (turn_count(board) % 2 == 0) {
		new_board[0] |= move;
	}
	else {
		new_board[1] |= move;
	}

	//Save move memory
	if(new_board[2] >> 60 != 0)
	{
		new_board[3] |= column << 3 * (turn_count(new_board) - 21);
	} else
	{
		new_board[2] |= column << 3 * turn_count(new_board);
	}
	
	//Save Move Height
	new_board[4] = (new_board[4] & ~(0b111ull << (column - 1) * 3)) | ((column_height + 1) << ((column - 1) * 3));

	//Save Move Count
	new_board[4] = (new_board[4] & ~(0b11111111ull << (8 * 7))) | ((unsigned long long)(turn_count(board) + 1)) << (8ull * 7ull);

	auto end = std::chrono::high_resolution_clock::now();
	move_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	return new_board;


}

std::array<std::array<unsigned long long, 5>, 7> Board::get_children(std::array<unsigned long long, 5> board)
{
	auto start = std::chrono::high_resolution_clock::now();

	std::array<std::array<unsigned long long, 5>, 7> boards{};
	auto end = std::chrono::high_resolution_clock::now();
	vector_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 7; i++)
		if (((board[4] >> (i * 3)) & 0b111) < 6) {
			boards[i] = (move(board, i + 1));
		}

	end = std::chrono::high_resolution_clock::now();
	child_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	return boards;
}

bool Board::evaluate_board(unsigned long long b)
{
	int directions[]{ 1, 7, 6, 8 };

	for(const int i : directions)
	{
		
		if ((b & (b >> i) & (b >> i * 2) & (b >> i * 3)) > 0)
		{
			scores[b] = 10001;
		}
		if ((b & (b >> i) & (b >> i * 2)) > 0)
		{
			if(scores[b] < 10001)
				scores[b] = 1001;
		}
		if ((b & (b >> i)) > 0)
		{
			if (scores[b] < 1001)
				scores[b] = 101;
		}
	}
	scores[b] = 1;
	return scores[b];
}

int Board::evaluate_board(std::array<unsigned long long, 5> board)
{

	const auto start = std::chrono::high_resolution_clock::now();
	auto x_num = scores[board[0]];
	auto o_num = scores[board[1]];
	if (x_num != 0 && o_num != 0)
	{
		const auto end = std::chrono::high_resolution_clock::now();
		eval_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		return (x_num - 1) - (o_num - 1);
	}
	if(x_num != 0)
	{
		o_num = evaluate_board(board[1]);
		const auto end = std::chrono::high_resolution_clock::now();
		eval_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		return (x_num - 1) - (o_num - 1);
	}
	if (o_num != 0)
	{
		x_num = evaluate_board(board[0]);
		o_num = evaluate_board(board[1]);
		const auto end = std::chrono::high_resolution_clock::now();
		eval_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		return (x_num - 1) - (o_num - 1);
	}
	x_num = evaluate_board(board[0]);
	const auto end = std::chrono::high_resolution_clock::now();
	eval_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	return (x_num - 1) - (o_num - 1);
	
}

std::pair<std::array<unsigned long long, 5>, int> Board::minimax(std::array<unsigned long long, 5> board, int depth,
                                                                 bool max_player, int max_depth)
{
	counter++;

	//if (counter % 500000 == 0)
	//{
	//	print_board(board);
	//	std::cout << counter << std::endl;
	//}
	if (turn_count(board) >= 7) {
		auto score = (float) evaluate_board(board);
		if (score != 0 || depth > max_depth)
		{
			return { board, score };
		}
	}
	std::array<unsigned long long, 5> move = { 0, 0, 0, 0, 0 };
	int value;
	if (max_player)
	{
		value = -1000000;
		for (std::array<unsigned long long, 5> b : get_children(board))
		{
			if (b[0] != 0) {
				const auto result = minimax(b, depth + 1, !max_player, max_depth);
				if (result.second / depth > value) {
					value = result.second;
					move = b;
				}
				else if (result.second / depth == value)
				{
					move = rand() % 2 == 0 ? b : move;
				}
			}
		}
		return { move, value };
	}
	else
	{
		value = 1000000;
		for (const std::array<unsigned long long, 5>& b : get_children(board))
		{
			if (b[0] != 0) {
				const auto result = minimax(b, depth + 1, !max_player, max_depth);
				if (result.second / depth < value) {
					value = result.second;
					move = b;
				}
				else if (result.second / depth == value)
				{
					move = rand() % 2 == 0 ? b : move;
				}
			}
		}
		return { move, value };
	}
}

