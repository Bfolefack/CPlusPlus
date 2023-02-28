#include "Node.h"
#include "NeatConfig.h"
std::array<int, 9> NeatConfig::activation_weights = {1, 1, 1, 1, 1, 1, 1, 1, 1};
float NeatConfig::compatibility_threshold = 1.f;

Node::Node(int innovation, int x, int activation)
{
    innovation_number = innovation;
    xPos = x;
    activation_function = activation;
    weightedSum = 0;
    activatedSum = 0;
}

bool Node::operator<(const Node& right) const
{
    if (xPos == right.xPos)
    {
        return innovation_number < right.innovation_number;
    }
    return  xPos < right.xPos;
}

int Node::get_activation()
{
	int sum = 0;

	
	int num = rand() % NeatConfig::activation_weights_sum;
	for (int i = 0; i < NeatConfig::activation_weights.size(); ++i)
	{
		sum += NeatConfig::activation_weights[i];
		if (sum > num)
		{
			return i;
		}
	}
	return 8;
}

void Node::activate()
{

	switch (activation_function)
	{
	case 0:
		//Sigmoid
		activatedSum = 1 / (1 + exp(-weightedSum));
		break;
	case 1:
		//Tanh
		activatedSum = tanh(weightedSum);
		break;
	case 2:
		//Relu
		activatedSum = weightedSum > 0 ? weightedSum : 0;
		break;
	case 3:
		//Leaky Relu
		activatedSum = weightedSum > 0.f ? weightedSum : 0.01f * weightedSum;
		break;
	case 4:
		//Linear
		activatedSum = weightedSum;
		break;
	case 5:
		//Gaussian
		activatedSum = exp(-pow(weightedSum, 2));
		break;
	case 6:
		//Sinusoid
		activatedSum = sin(weightedSum);
		break;
	case 7:
		//Softplus
		activatedSum = log(1 + exp(weightedSum));
		break;
	case 8:
		//Absolute Value
		activatedSum = abs(weightedSum);
		break;
	}
	weightedSum = 0;
}

Node Node::copy() const
{
	auto node = Node{ innovation_number, xPos , activation_function};
	node.weightedSum = 0;
	node.activatedSum = 0;
	for (auto c : outgoingConnections)
	{
		node.outgoingConnections.push_back(c);
	}
	return  node;
}
