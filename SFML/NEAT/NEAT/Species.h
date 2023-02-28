#pragma once
#include <array>
#include <memory>
#include <unordered_map>

#include "NeatActor.h"
#include "WeightedRandomSelectMap.h"


template <class A> class Species
{
public:
	WeightedRandomSelectMap<int, std::shared_ptr<A>> actors;
	std::shared_ptr<A> representative;
	float best_fitness;
	float high_score_fitness;
	int staleness;

	Species(std::shared_ptr<A> founder);
	bool add(const std::shared_ptr<A>& actor);
	bool is_compatible(const std::shared_ptr<A>& interloper);
	void clear();
	void cull();
	auto reproduce(float x);
	void sum_weights();
	static std::pair<float, float> get_mismatch(std::shared_ptr<A> actor1, std::shared_ptr<A> actor2);
	std::shared_ptr<A> get_random_actor();

	bool operator< (const Species& right) const;
};

template <class A>
Species<A>::Species(std::shared_ptr<A> founder)
{
	auto rep = A{ founder->genome };
	representative = std::make_shared<A>(rep);
	representative->batch_fitness = founder->batch_fitness;
	best_fitness = founder->batch_fitness;
	high_score_fitness = founder->batch_fitness;
	staleness = 0;
	actors.insert(founder->id, founder, founder->batch_fitness);
}

template <class A>
bool Species<A>::add(const std::shared_ptr<A>& actor)
{
	
	if (actor == nullptr)
		return false;

	if (is_compatible(actor))
	{
		actors.place(actor->id, actor, actor->batch_fitness);
		return true;
	}
	return false;
}

template <class A>
bool Species<A>::is_compatible(const std::shared_ptr<A>& interloper)
{
	if (representative->size() == 0 && interloper->size() == 0)
		return true;
	auto mismatches = get_mismatch(representative, interloper);
	
	float flatten = interloper->genome.connections.size() - NeatConfig::large_network_threshold;

	if (flatten < 1)
		flatten = 1;
	auto compatibility = (NeatConfig::mismatch_coefficient * mismatches.first / flatten) + (mismatches.second * NeatConfig::weight_coefficient);
	//std::cout << compatibility << std::endl;
	return NeatConfig::compatibility_threshold > compatibility;
}

template <class A>
void Species<A>::clear()
{
	actors = WeightedRandomSelectMap<int, std::shared_ptr<A>>{};
	
}

template <class A>
void Species<A>::cull()
{

	if(actors.size() <= 2)
	{
		//staleness = 10000;
		staleness++;
		return;
	}
	const int cull = static_cast<int>(floorf(actors.size() * NeatConfig::cull_rate));
	actors.pop_worst(cull);
	best_fitness = actors.get_best()->second->batch_fitness;
	if (actors.get_best()->second->batch_fitness > representative->batch_fitness)
	{
		auto rep = A{ actors.get_best()->second->genome };
		representative = std::make_shared<A>(rep);
		representative->batch_fitness = best_fitness;
		
		staleness = 0;
	}
	else
	{
		staleness++;
	}
}

template <class A>
auto Species<A>::reproduce(float x)
{
	std::unordered_set<A*> children;
	for (int i = 0; i < x; i++)
	{
		auto parent1 = actors.get_random().second;
		auto parent2 = actors.get_random().second;
		if (parent1->batch_fitness < parent2->batch_fitness)
		{
			auto temp = parent1;
			parent1 = parent2;
			parent2 = temp;
		}
		A* temp = parent1->breed(&(*parent2));
		children.insert(temp);
	}
	return children;
}

template <class A>
void Species<A>::sum_weights()
{
	actors.sum_weights();
}

template <class A>
std::pair<float, float> Species<A>::get_mismatch(std::shared_ptr<A> actor1, std::shared_ptr<A> actor2)
{
	std::unordered_set<int> disjoint;
	std::unordered_set<int> match;
	
	for (const auto& gene : actor1->genome.connections)
	{
		disjoint.insert(gene.first);
	}
	for (const auto& gene : actor2->genome.connections)
	{
		if (disjoint.find(gene.first) != disjoint.end())
		{
			disjoint.erase(gene.first);
			match.insert(gene.first);
		} else
		{
			disjoint.insert(gene.first);
		}
	}
	const float mismatch = disjoint.size();
	
	if (match.empty())
		return { mismatch, 500 };
	float weight_mismatch = 0;
	for(const int i : match)
	{
		weight_mismatch += abs(actor1->genome.connections[i]->weight - actor2->genome.connections[i]->weight);
	}
	return { mismatch, weight_mismatch / match.size() };

}

template <class A>
std::shared_ptr<A> Species<A>::get_random_actor()
{
	return actors.get_random().second;
}

template <class A>
bool Species<A>::operator<(const Species& right) const
{
	return representative->batch_fitness < right.representative->batch_fitness;
}
