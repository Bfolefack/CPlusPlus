#include <chrono>
#include <iostream>

#include "Genome.h"
#include "Population.h"
#include "XORActor.h"

int main() {
	srand(time(NULL));
	//std::set<std::shared_ptr<Node>> nodes;
	//nodes.insert(std::make_shared<Node>(Node(12, 0)));
	//nodes.insert(std::make_shared<Node>(Node(10, 0)));
	//nodes.insert(std::make_shared<Node>(Node(5, 1)));
	//nodes.insert(std::make_shared<Node>(Node(7, 1)));
	//nodes.insert(std::make_shared<Node>(Node(3, 2)));
	//for(auto n : nodes)
	//	std::cout << n->innovation_number << ", " << n->xPos << std::endl;

	//Genome g1{};
	////Genome g2{};
	//for (int i = 0; i < 500; ++i)
	//{
	//	if(rand() % 4 < 3)
	//	{
	//		g1.add_connection();
	//	} else
	//	{
	//		g1.add_node();
	//	}		
	//}
	//for (int i = 0; i < 500; ++i)
	//{
	//	g1.add_connection();
	//}

	//auto out = g1.feed_forward({ 1, 0.5, 1 });
	//for (auto i : out)
	//{
	//	std::cout << i  << ", ";
	//}
	//std::cout << std::endl;

	//auto out2 = g2.feed_forward({ 1, 0.5, 1 });
	//for (auto i : out2)
	//{
	//	std::cout << i << ", ";
	//}
	//std::cout << std::endl;

	//auto child = Genome::crossover(g1, g2);
	//auto out3 = child.feed_forward({ 1, 0.5, 1 });
	//for (auto i : out3)
	//{
	//	std::cout << i << ", ";
	//}

	//for (float k = 0.9; k >= 0; k -= 0.01)
	//{
	//	for (float l = 0; l < 1; l += 0.01)
	//	{
	//		std::cout << (int)(g1.feed_forward({ k, l })[0] * 10);
	//		//std::cout << (int)((abs(k - l) * 2 - 1) *10);
	//	}
	//	std::cout << std::endl;
	//}

	Population<XORActor> pop{500};
	pop.generation();


	std::cout << "Population created" << std::endl;
	for (int j = 0; j < 10000; ++j) {
		auto start = std::chrono::high_resolution_clock::now();
		pop.act();
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "Acting " << j << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
		
		if (j % 5 == 0) {
			for (float k = 0.9; k >= 0; k -= 0.01)
			{
				for (float l = 0; l < 1; l += 0.01)
				{
					std::cout << (int)(pop.best_actor->genome.feed_forward({ k, l })[0] *  10) ;
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
			for (float k = 0.9; k >= 0; k -= 0.01)
			{
				for (float l = 0; l <= 1; l += 0.01)
				{
					std::cout << (int)(pop.actors.begin()->second->genome.feed_forward({k, l})[0] * 10);
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
			for (float k = 0.9; k >= 0; k -= 0.01)
			{
				for (float l = 0; l <= 1; l += 0.01)
				{
					//std::cout << (int)(sqrt((k - 0.5) * (k - 0.5) +  (l - 0.5) * (l - 0.5))* 10);
					std::cout << (int)(abs(k - l) * 10);
				}
				std::cout << std::endl;
			}
		}
		

		start = std::chrono::high_resolution_clock::now();
		pop.epoch();
		end = std::chrono::high_resolution_clock::now();
		if (pop.epoch_num != 0)
			std::cout << "Epoch " << j << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
		else
		{
			std::cout << "Generation " << j << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
		}
		std::cout << std::endl;
	}


}