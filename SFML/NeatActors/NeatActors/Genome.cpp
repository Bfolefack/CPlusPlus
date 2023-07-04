#include "Genome.h"
#include <chrono>
#include "NeatConfig.h"


int Genome::next_node = NeatConfig::max_outputs + NeatConfig::max_inputs;
BiMap<int, int> Genome::node_innovations{};
std::unordered_set<int> Genome::connection_innovations{};

Genome::Genome()
{
	layers = 0;
}

Genome::Genome(int number)
{
	layers = 1;
	for (int i = 0; i < NeatConfig::max_inputs + 1; ++i)
	{
		Node n{ i, 0, 0 };
		nodes.insert(n.innovation_number, std::make_shared<Node>(n));
	}
	for (int i = NeatConfig::max_inputs + 1; i < NeatConfig::max_inputs + 1 + NeatConfig::max_outputs; ++i)
	{
		Node n{ i, 1, 0 };
		nodes.insert(n.innovation_number, std::make_shared<Node>(n));
	}
	for (int i = 0; i < NeatConfig::max_inputs + 1; ++i)
	{
		for (int j = NeatConfig::max_inputs + 1; j < NeatConfig::max_inputs + 1 + NeatConfig::max_outputs; ++j)
		{
			add_connection(i, j);
		}
	}
}

void Genome::connect_network()
{
	network.clear();
	for (const auto& node : nodes)
	{
		node.second->outgoingConnections.clear();
		network.insert(NodeGene{ node.second->innovation_number, node.second->xPos });
	}
	for(const auto& connection : connections)
	{
		nodes[connection.second->in]->outgoingConnections.push_back(connection.first);
	}
}

void Genome::add_node()
{
	while (connections.size() < 1)
	{
		add_connection();
	}

	auto og = connections.get_random();
	if (og.second->disabled)
		return;
	const int activation = Node::get_activation();
	int innovation;
	if (node_innovations.find_left(og.first * 100 + activation) == node_innovations.end_left())
	{
		node_innovations.insert(og.first * 100 + activation, next_node);
		innovation = next_node;
		next_node++;
	}
	else
	{
		innovation = node_innovations.find_left(og.first * 100 + activation)->second;
	}
	auto in_layer = nodes[og.second->in]->xPos;
	auto out_layer = nodes[og.second->out]->xPos;
	int xPos = -1;
	if(abs(in_layer - out_layer) > 1)
	{
		xPos = (in_layer + out_layer) / 2;
	} else
	{
		if (in_layer > out_layer)
		{
			const auto temp = out_layer;
			out_layer = in_layer;
			in_layer = temp;
		}
		xPos = out_layer;
		for (const auto& node : nodes)
		{
			if (node.second->xPos >= out_layer)
				node.second->xPos++;

		}
		layers++;
	}
	if (innovation != -1) {
		Node n{ innovation, xPos, activation };
		nodes.insert(innovation, std::make_shared<Node>(n));
		og.second->disabled = true;
		add_connection(og.second->in, innovation, 1);
		add_connection(innovation, og.second->out, og.second->weight);
		//if (nodes[og.second->in])

		connect_network();
	} else
	{
		std::cout << "Invalid Innovation";
	}
}



void Genome::mutate()
{
	if((rand() % 1000)/1000.f < NeatConfig::add_connection_rate)
	{
		add_connection();
	}
	if ((rand() % 1000) / 1000.f < NeatConfig::add_node_rate)
	{
		add_node();
	}
	for (const auto& connection : connections)
	{
		if ((rand() % 1000) / 1000.f < NeatConfig::perturb_weight_rate)
		{
			perturb_weight(connection.first);
		}
		if ((rand() % 1000) / 1000.f < NeatConfig::scramble_weight_rate)
		{
			scramble_weight(connection.first);
		}
		if ((rand() % 1000) / 1000.f < NeatConfig::disable_enable_connection_rate)
		{
			disable_enable_connection(connection.first);
		}
	}
	for (const auto& node : nodes)
	{
		if ((rand() % 1000) / 1000.f < NeatConfig::disable_enable_node_rate)
		{
			disable_enable_node(node.first);
		}
	}

}

void Genome::add_connection(int node1, int node2, float weight)
{
	const auto connection_id = node1 * NeatConfig::max_nodes + node2;
	if (connections.find(connection_id) == connections.end())
	{
		return;
	}
	if (connection_innovations.find(connection_id) != connection_innovations.end())
	{
		connections[connection_id] = std::make_shared<Connection>(Connection{ node1, node2, connection_id, weight });
	}
	else
	{
		connection_innovations.insert(connection_id);
		connections[connection_id] = std::make_shared<Connection>(Connection{ node1, node2, connection_id, weight });
	}
	connect_network();
}

void Genome::add_connection(int node1, int node2)
{
	const auto connection_id = node1 * NeatConfig::max_nodes + node2;
	connections.insert(connection_id, std::make_shared<Connection>(Connection{ node1, node2, connection_id, (rand() % 2000) / 1000.0f - 1.0f }));
	connect_network();
}

void Genome::add_connection()
{
	std::shared_ptr<Node> node1;
	std::shared_ptr<Node> node2;
	do {
		node1 = nodes.get_random().second;
		node2 = nodes.get_random().second;
	} while (node2->xPos == 0 || node1->xPos == layers || node1->xPos >= node2->xPos);

	const auto connection_id = node1->innovation_number * NeatConfig::max_nodes + node2->innovation_number;
	if (connections.find(connection_id) == connections.end())
	{
		if (connection_innovations.find(connection_id) != connection_innovations.end())
		{
			connections.insert(connection_id, std::make_shared<Connection>(Connection{ node1->innovation_number, node2->innovation_number, connection_id, (rand() % 2000) / 1000.0f - 1.0f, true}));
		}
		else
		{
			connection_innovations.insert(connection_id);
			connections.insert(connection_id, std::make_shared<Connection>(Connection{ node1->innovation_number, node2->innovation_number, connection_id, (rand() % 2000) / 1000.0f - 1.0f, true}));
		}
	}	
}

void Genome::disable_enable_connection(int id)
{
	const auto& connection = connections[id];
	connection->disabled = !connection->disabled;
}

void Genome::disable_enable_node(int id)
{
	const auto& node = nodes[id];
	for (const auto& connection : connections)
	{
		if (connection.second->in == node->innovation_number || connection.second->out == node->innovation_number)
		{
			connection.second->disabled = !connection.second->disabled;
		}
	}
}

void Genome::perturb_weight(const int& id)
{
	if (!connections[id]->disabled) {
		const auto perturbation = (1 - NeatConfig::weight_perturbation) + ((rand() % 2000) / 1000.0f) * NeatConfig::weight_perturbation;
		connections[id]->weight *= perturbation;
	}
}

void Genome::scramble_weight(const int& id)
{
	const auto& connection = connections[id];
	if (!connection->disabled)
		connection->weight = (rand() % 2000) / 1000.0f - 1.0f;
}

Genome Genome::crossover(Genome& dominant, Genome& recessive)
{
	Genome child{};
	child.layers = dominant.layers;

	for (const auto& connection : dominant.connections)
	{
		bool enabled = false;
		if (recessive.connections.find(connection.first) != recessive.connections.end())
		{
			if (connection.second->disabled || recessive.connections[connection.first]->disabled)
			{
				//TODO: ADD TO CONFIG
				if ((rand() % 100) / 100.0f < 0.5f)
					enabled = true;
			} else
			{
				enabled = true;
			}

			if ((rand() % 100)/100.f > 0.5f)
			{
				child.connections.insert(connection.first, std::make_shared<Connection>(Connection{
					connection.second->in,
					connection.second->out,
					connection.second->innovationNum,
					connection.second->weight,
					enabled
				}));
			}
			else
			{
				child.connections.insert(connection.first, std::make_shared<Connection>(Connection{
					recessive.connections[connection.first]->in,
					recessive.connections[connection.first]->out,
					recessive.connections[connection.first]->innovationNum,
					recessive.connections[connection.first]->weight,
					enabled
				}));
			}
		}
		else
		{
			child.connections.insert(connection.first, std::make_shared<Connection>(Connection{
					connection.second->in,
					connection.second->out,
					connection.second->innovationNum,
					connection.second->weight,
					connection.second->disabled
			}));
		}
	}
	for (const auto& node : dominant.nodes)
	{
		child.nodes.insert(node.first, std::make_shared<Node>(node.second->copy()));
	}

	child.connect_network();

	return child;
}

void Genome::reset()
{
	for (const auto& node : nodes)
	{
		node.second->activatedSum = 0;
		node.second->weightedSum = 0;
	}
}

std::vector<float> Genome::feed_forward(const std::vector<float>& input_values)
{
	nodes[0]->activatedSum = 0;
	for (int i = 1; i <= input_values.size(); ++i)
	{
		nodes[i]->activatedSum = input_values[i - 1];
	}
	for (const auto& node : network)
	{
		if (node.xPos > 0) {
			nodes[node.innovation_number]->activate();
		}
		for (const auto connection : nodes[node.innovation_number]->outgoingConnections)
		{
			//if(nodes[node.innovation_number]->activatedSum != 0)
			//	std::cout << nodes[node.innovation_number]->activatedSum << std::endl;
			nodes[connection % NeatConfig::max_nodes]->weightedSum += connections[connection]->disabled ?  0 : nodes[node.innovation_number]->activatedSum * connections[connection]->weight;
		}
	}


	auto out = std::vector<float>();
	for (int i = NeatConfig::max_inputs + 1; i < NeatConfig::max_inputs + 1 + NeatConfig::max_outputs; ++i)
	{
		out.push_back(nodes[i]->activatedSum);
	}

	return out;
}
