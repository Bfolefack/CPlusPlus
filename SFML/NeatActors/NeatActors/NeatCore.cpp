#include "NeatCore.h"

#include "NeatConfig.h"

double NeatCore::fitness_noise()
{
	return (rand() % 100) / 100.f + (rand() % 100) / (100.f * 100.f) + (rand() % 100) / (100.f * 100.f * 100.f) + (rand() % 100) / (100.f * 100.f * 100.f * 100.f);
}

NeatCore::NeatCore()
{
	deprecated_id = -1;
	epoch_fitness = 0;
	batch_fitness = 0;
	alive = true;
	//genome = Genome();
}

NeatCore::NeatCore(const Genome& g)
{
	deprecated_id = -1;
	epoch_fitness = 0;
	batch_fitness = 0;
	alive = true;
	genome = g;
}



bool NeatCore::operator<(const NeatCore& right) const
{
	return batch_fitness < right.batch_fitness;
}



std::tuple<float, float, float> NeatCore::get_mismatch(std::shared_ptr<NeatCore> interloper)
{
	std::unordered_set<int> disjoint;
	std::unordered_set<int> match;

	for (const auto& gene : genome.connections)
	{
		disjoint.insert(gene.first);
	}
	for (const auto& gene : interloper->genome.connections)
	{
		if (disjoint.find(gene.first) != disjoint.end())
		{
			disjoint.erase(gene.first);
			match.insert(gene.first);
		}
		else
		{
			disjoint.insert(gene.first);
		}
	}
	const float mismatch = disjoint.size();

	if (match.empty())
		return { mismatch, 500, 0};
	float weight_mismatch = 0;
	for (const int i : match)
	{
		weight_mismatch += abs(genome.connections[i]->weight - interloper->genome.connections[i]->weight);
	}
	return { mismatch, weight_mismatch / (float) match.size(), 0};

}