#include "NeatActor.h"

double NeatActor::fitness_noise()
{
	return (rand() % 100) / 100.f + (rand() % 100) / (100.f * 100.f) + (rand() % 100) / (100.f * 100.f * 100.f) + (rand() % 100) / (100.f * 100.f * 100.f * 100.f);
}

NeatActor::NeatActor()
{
	id = -1;
	epoch_fitness = 0;
	batch_fitness = 0;
	alive = true;
	genome = Genome();
}

NeatActor::NeatActor(const Genome& g)
{
	id = -1;
	epoch_fitness = 0;
	batch_fitness = 0;
	alive = true;
	genome = g;
}



bool NeatActor::operator<(const NeatActor& right) const
{
	return batch_fitness < right.batch_fitness;
}
