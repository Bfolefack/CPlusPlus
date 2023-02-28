#include "XORActor.h"

#include <chrono>


void XORActor::set_inputs(std::vector<float> inputs)
{
}

void XORActor::act()
{

	//const float left = (rand() % 1000)/1000.f;
	//const float right = (rand() % 1000)/1000.f;
	const float left = (rand() % 2);
	const float right = (rand() % 2);
	//const float answer = sqrt((left - 0.5) * (left - 0.5) + (right - 0.5) * (right - 0.5)) ;
	const float answer = abs(left - right);
	
	auto out = genome.feed_forward({ (float)left, (float)right });

	if (out[0] > 1)
		out[0] = 1;
	else if(out[0] < 0)
		out[0] = 0;
	const auto error = abs(out[0] - answer);
	epoch_fitness += 1 - error * error;

	
}

XORActor* XORActor::breed(NeatActor* other)
{
	Genome new_genome = Genome::crossover(genome, other->genome);
	new_genome.mutate();
	return new XORActor{ new_genome };
}

double XORActor::calculate_fitness()
{
	batch_fitness = (batch_fitness * batch_fitness * batch_fitness + fitness_noise() * 0.1) /(genome.connections.size() + 100);
	return batch_fitness;
}

void XORActor::normalize_fitness()
{
	//batch_fitness /= NeatConfig::epochs_per_batch;
}

void XORActor::epoch()
{
	batch_fitness += epoch_fitness;
	epoch_fitness = 0;
}

XORActor* XORActor::clone()
{
	XORActor out{ this->genome };
	return &out;
}

int XORActor::size()
{
	return genome.connections.size();
}

XORActor::XORActor() : NeatActor() {}

XORActor::XORActor(const Genome& g) : NeatActor(g) {}
