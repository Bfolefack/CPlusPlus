#include <array>
#include <iostream>
#include <bitset>
#include "Board.h"

//Child Time : 674953118
//Eval Time : 663657
//Move Time : 108369195
//100,000,000

//Child Time : 4,371,735
//Eval Time : 164,442
//Move Time : 41,551
//Move Count : 10,000,000

//Child Time : 5,034,246
//Eval Time : 20,5852
//Move Time : 44,443
//Move Count : 10,000,000

//Child Time : 4,477,968
//Eval Time : 158,771
//Move Time : 40,942
//Move Count : 10,000,000

//Child Time : 3,372,971
//Eval Time : 165,376
//Move Time : 45,731
//Move Count : 10,000,000

//Child Time : 3,696,209
//Eval Time : 100,505
//Move Time : 17,017
//Move Count : 10,000,000

//Child Time : 3,763,059
//Eval Time : 100,381
//Move Time : 17,187
//Move Count : 10,000,000

//Child Time : 539,119
//Eval Time : 106,657
//Move Time : 18,540
//Vector Time : 210,919
//Move Count : 10,000,000

int main() {
	std::array<unsigned long long, 5> board{ 0, 0, 0, 0, 0 };

    auto game = Board::new_board();
    bool AITurn = true;

    while (true) {
        if (AITurn) {
            Board::print_board(game);
            auto choice = Board::minimax(game, 1, true, 8);
            std::cout << "Gain: " << choice.second << std::endl;
            game = choice.first;
            if (choice.second >= 10000)
                std::cout << "AI Player Wins!!!" << std::endl;
            AITurn = !AITurn;
        }
        else {
            Board::print_board(game);
            int x;
            std::cout << "Your Move" << std::endl << std::endl;
            std::cin >> x;
            game = Board::move(game, x);
            AITurn = !AITurn;
        }
    }
}
