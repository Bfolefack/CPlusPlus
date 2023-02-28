#pragma once
#include <unordered_set>
#include <vector>

class Node
{
	public:

	float weightedSum;
	float activatedSum;

	int innovation_number;
	int xPos;
	int activation_function;

	

	std::vector<int> outgoingConnections;

	Node(int innovation, int x);

	Node(int innovation, int x, int activation);
	/*friend bool operator< (Node& left, Node& right);*/
	bool operator< (const Node& right) const;
	static int get_activation();
	void activate();
	Node copy() const;
};
