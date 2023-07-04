#pragma once
#include <memory>
#include <set>

#include <unordered_set>

#include "Connection.h"
#include "Node.h"
#include "NodeGene.h"
#include "BiMap.h"
#include "RandomSelectMap.h"

class Genome
{
public:


	static BiMap<int, int> node_innovations;
	static std::unordered_set<int> connection_innovations;
	static int next_node;

	std::set <NodeGene> network;
	RandomSelectMap<int, std::shared_ptr<Node>> nodes;
	RandomSelectMap<int, std::shared_ptr<Connection>> connections;

	int layers;

	Genome();
	Genome(int number);

	//Genome Construction Functions
	void connect_network();
	void add_connection(int node1, int node2, float weight);
	void add_connection(int node1, int node2);


	//Mutation Functions
	void mutate();
	void add_node();
	void add_connection();
	void disable_enable_connection(int id);
	void disable_enable_node(int id);
	void perturb_weight(const int& id);
	void scramble_weight(const int& id);

	//Reproduction Functions
	static Genome crossover(Genome& dominant, Genome& recessive);

	void reset();
	std::vector<float> feed_forward(const std::vector<float>& input_values);

};
