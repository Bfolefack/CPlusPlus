#pragma once
#include "NeatActor.h"
#include "PhysicsActor.h"



class Actor : public NeatActor, public PhysicsActor
{
public:
	std::vector<std::shared_ptr<Actor>> sighted_actors;
	std::vector<std::shared_ptr<Actor>> for_creation;

	std::vector<float> hyper_parameters;

	using PhysicsActor::PhysicsActor;
	using NeatActor::NeatActor;
	Actor();
	Actor(Ball& b);
	Actor(Ball& b, int s);
	Actor(const Genome& g);
	Actor(const Ball& b, const Genome& g);

	void set_inputs(std::vector<float> inputs) override;
	void act() override;
	Actor* breed(NeatActor* other) override;
	double calculate_fitness() override;
	void normalize_fitness() override;
	void epoch() override;
	Actor* clone() override;
	int size() override;

	void update(float delta_time) override;
	int hit(sf::Vector2f source, float angle, float range,
	        const std::unordered_map<int, std::shared_ptr<Actor>>& actors) const;
	void ray_cast(const std::unordered_map<int, std::shared_ptr<Actor>>& actors);
};

