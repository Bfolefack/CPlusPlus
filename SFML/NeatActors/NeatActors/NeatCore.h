#pragma once
#include <vector>

#include "Genome.h"

class NeatCore
{
public:
	virtual ~NeatCore() = default;
	double epoch_fitness;
	int deprecated_id;
	Genome genome;
	double batch_fitness;
	bool alive;
	std::vector<float> inputs;


	virtual void set_inputs(std::vector<float> inputs) = 0;
	virtual void act() = 0;
	virtual Genome breed(NeatCore* other) = 0;
	virtual double calculate_fitness() = 0;
	virtual void normalize_fitness() = 0;
	virtual void epoch() = 0;
	virtual NeatCore* clone() = 0;
	virtual int get_network_size() = 0; //actor.genome.connections.size
	static double fitness_noise();

	NeatCore();
	NeatCore(const Genome& g);
	
	bool operator< (const NeatCore& right) const;
	virtual std::tuple<float, float, float> get_mismatch(std::shared_ptr<NeatCore> interloper);
};
