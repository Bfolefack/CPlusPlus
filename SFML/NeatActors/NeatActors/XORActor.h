#pragma once
#include "NeatCore.h"
class XORActor : public NeatCore
{
public:

	void set_inputs(std::vector<float> inputs) override;
	void act() override;
	Genome breed(NeatCore* other) override;
	double calculate_fitness() override;
	void normalize_fitness() override;
	void epoch() override;
	XORActor* clone() override;
	int get_network_size() override;

	XORActor();
	XORActor(const Genome& g);
};

