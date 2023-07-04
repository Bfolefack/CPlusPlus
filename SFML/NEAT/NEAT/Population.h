#pragma once
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "NeatConfig.h"
#include "Species.h"

template <class A> class Population
{
public:
	std::unordered_map<int, std::shared_ptr<A>> actors;
	std::set<std::shared_ptr<Species<A>>> species;
	float best_fitness;
	float high_score_fitness;
	float average_fitness;
	int generation_num;
	int epoch_num;
	int population_size;
	std::shared_ptr<A> best_actor;

	Population(int population_size);
	Population(std::unordered_set<std::shared_ptr<A>> founders);

	void act();
	std::shared_ptr<A> alive();
	
	void generation();
	void reproduce(float mass_extinction);
	void sort_species();
	void epoch();
	void calculate_fitness();

};

template <class A>
Population<A>::Population(int pop)
{

	for (int i = 0; i < NeatConfig::max_inputs + 1; ++i)
	{
		for (int j = NeatConfig::max_inputs + 1; j < NeatConfig::max_inputs + 1 + NeatConfig::max_outputs; ++j)
		{
			
			const auto connection_id = i * NeatConfig::max_nodes + j;
			Genome::connection_innovations.insert(connection_id);
		}
	}
	for (int i = 0; i < pop; i++)
	{
		actors[i] = std::make_shared<A>();
		actors[i]->id = i;
	}
	population_size = pop;

}

template <class A>
Population<A>::Population(std::unordered_set<std::shared_ptr<A>> founders)
{
	best_fitness = 0;
	high_score_fitness = 0;
	average_fitness = 0;
	generation_num = 0;
	epoch_num = 0;
	population_size = founders.size();
	for (const auto actor : founders)
	{
		actors[actor->id] = actor;
	}
}

template <class A>
void Population<A>::act()
{
	for (const auto& actor : actors)
	{
		actor.second->act();
	}
}

template <class A>
std::shared_ptr<A> Population<A>::alive()
{
	for (const auto& actor : actors)
	{
		if (actor.second->alive)
			return actor.second;
	}
	return nullptr;
}

template <class A>
void Population<A>::reproduce(float mass_extinction)
{
	WeightedRandomSelectMap<int, std::shared_ptr<A>> temp_map;
	temp_map.add_all(actors.begin(), actors.end());
	//auto start = std::chrono::high_resolution_clock::now();
	std::unordered_map<int, std::shared_ptr<A>> children;
	int id = 0;
	for (auto spec : species)
	{
		for (auto child : spec->reproduce(spec->actors.map.size()* NeatConfig::inbreeding_percentage * 2 * mass_extinction))
		{
			if (children.size() >= population_size)
				break;
			auto temp = *child;
			temp.id = id;
			children[id] = std::make_shared<A>(temp);
			id++;
		}
	}
	//auto end = std::chrono::high_resolution_clock::now();
	//std::cout << "Species Breading: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

	//start = std::chrono::high_resolution_clock::now();
	while (children.size() < population_size)
	{
		auto parent1 = temp_map.get_random().second;
		auto parent2 = temp_map.get_random().second;
		if (parent1->batch_fitness < parent2->batch_fitness)
		{
			auto temp = parent1;
			parent1 = parent2;
			parent2 = temp;
		}
		A temp = *parent1->breed(&(*parent2));
		temp.id = id;
		children[id] = std::make_shared<A>(temp);
		id++;
	}
	//end = std::chrono::high_resolution_clock::now();
	//std::cout << "Interspecies Breading: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
	actors = children;
	std::cout << std::endl;
}

template <class A>
void Population<A>::generation()
{

	generation_num++;
	float mass_extinciton = 1;
	if((rand() % 1000)/1000.f < NeatConfig::mass_extinction_rate)
	{
		std::cout << "Mass Extinction!" << std::endl;
		mass_extinciton = 1.f/NeatConfig::mass_extinction_threshold;
		while (actors.size() > NeatConfig::mass_extinction_threshold * population_size)
		{
			auto ind = actors.begin();
			for (int i = 0; i < rand() % actors.size(); i++)
			{
				++ind;
			}
			actors.erase(ind);
		}
	}
	//auto start = std::chrono::high_resolution_clock::now();
	calculate_fitness();
	//auto end = std::chrono::high_resolution_clock::now();
	//std::cout << "Calculating Fitness: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

	//std::unordered_map<int, std::shared_ptr<NeatActor>> actors;
	//if((rand() % 1000)/1000.f < NeatConfig::mass_extinction_threshold)
	//{
	//	while()
	//}


	//start = std::chrono::high_resolution_clock::now();
	sort_species();
	//end = std::chrono::high_resolution_clock::now();
	//std::cout << "Sorting Species: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl << std::endl;
	
	std::set<std::shared_ptr<Species<A>>> good;
	int preCull = species.size();
	for (auto spec : species)
	{
		if (spec->actors.map.size() > 0 && spec->staleness < 15)
		{
			good.insert(spec);
		}
		if (species.size() < 2)
			good.insert(spec);
	}
	species = good;
	const int postCull = species.size();
	std::cout << "Culled " << (preCull - postCull) << " species" << std::endl;

	//start = std::chrono::high_resolution_clock::now();
	best_fitness = 0;
	for (auto spec : species)
	{
		spec->cull();
		if (spec->best_fitness >= best_fitness)
		{
			best_fitness = spec->best_fitness;
			if (spec->actors.get_best()->second != nullptr)
			{
				best_actor = spec->actors.get_best()->second;
			}
		}
	}
	//end = std::chrono::high_resolution_clock::now();
	//std::cout << "Culling Species Actors: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

	if (best_fitness > high_score_fitness)
	{
		high_score_fitness = best_fitness;
		std::cout << "NEW HIGHSCORE!" << std::endl;
	}

	std::cout << "Generation: " << generation_num << std::endl;
	std::cout << "Best fitness: " << best_fitness << std::endl;
	std::cout << "High score: " << high_score_fitness << std::endl;
	std::cout << "Species: " << species.size() << std::endl;
	if(species.size() < 5)
	{
		NeatConfig::compatibility_threshold *= 0.9f;
	}
	if (species.size() > 30)
	{
		NeatConfig::compatibility_threshold *= 1.1f;
	}


	reproduce(mass_extinciton);
}

template <class A>
void Population<A>::epoch()
{


	++epoch_num;
	for (const auto& actor : actors)
	{
		actor.second->epoch();
	}
	if(epoch_num >= NeatConfig::epochs_per_batch)
	{
		epoch_num = 0;
		generation();
		return;
	}
	
}

template <class A>
void Population<A>::calculate_fitness()
{
	for (const auto& actor : actors)
	{
		actor.second->calculate_fitness();
	}
}

template <class A>
void Population<A>::sort_species()
{
	for (auto& spec : species)
	{
		spec->clear();
	}

	//auto add_time = std::chrono::high_resolution_clock::duration::zero();
	//auto insert_time = std::chrono::high_resolution_clock::duration::zero();
	for(const auto& actor : actors)
	{
		bool added = false;

		//auto start = std::chrono::high_resolution_clock::now();
		for (auto& spec : species)
		{
			if(spec->add(actor.second))
			{
				added = true;
				break;
			}
		}
		//auto end = std::chrono::high_resolution_clock::now();
		//add_time += end - start;

		//start = std::chrono::high_resolution_clock::now();
		if(!added)
		{
			species.insert(std::make_shared<Species<A>>(Species<A>{ actor.second }));
		}
		//end = std::chrono::high_resolution_clock::now();
		//insert_time += end - start;
	}
	for (auto& spec : species)
	{
		spec->sum_weights();
	}
	//std::cout << "Add: " << std::chrono::duration_cast<std::chrono::milliseconds>(add_time).count() << "ms" << std::endl;
	//std::cout << "Insert: " << std::chrono::duration_cast<std::chrono::milliseconds>(insert_time).count() << "ms" << std::endl;
}
