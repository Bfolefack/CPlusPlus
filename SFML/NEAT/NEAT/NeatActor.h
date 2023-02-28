#pragma once
#include <vector>

#include "Genome.h"

class NeatActor
{
public:
	virtual ~NeatActor() = default;
	double epoch_fitness;
	int id;
	Genome genome;
	double batch_fitness;
	bool alive;
	std::vector<float> inputs;


	virtual void set_inputs(std::vector<float> inputs) = 0;
	virtual void act() = 0;
	virtual NeatActor* breed(NeatActor* other) = 0;
	virtual double calculate_fitness() = 0;
	virtual void normalize_fitness() = 0;
	virtual void epoch() = 0;
	virtual NeatActor* clone() = 0;
	virtual int size() = 0; //actor.genome.connections.size
	static double fitness_noise();

	NeatActor();
	NeatActor(const Genome& g);
	
	bool operator< (const NeatActor& right) const;
};
