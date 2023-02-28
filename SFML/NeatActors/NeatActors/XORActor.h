#pragma once
#include "NeatActor.h"
class XORActor : public NeatActor
{
public:

	void set_inputs(std::vector<float> inputs) override;
	void act() override;
	XORActor* breed(NeatActor* other) override;
	double calculate_fitness() override;
	void normalize_fitness() override;
	void epoch() override;
	XORActor* clone() override;
	int size() override;

	XORActor();
	XORActor(const Genome& g);
};

