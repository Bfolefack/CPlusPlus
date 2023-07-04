#pragma once
#include "NeatCore.h"
#include "PhysicsCore.h"



class Actor : public NeatCore, public PhysicsCore
{
public:
	std::vector<std::shared_ptr<Actor>> for_creation;
	std::shared_ptr<Actor> holding;
	bool brainless;
	int holder;
	float holder_str;
	float delta_time;

	std::vector<float> ray_angles;
	std::vector<std::shared_ptr<Actor>> sighted_actors;
	std::vector<float> sq_sighted_actor_dists;

	std::vector<float> outputs;

	std::vector<float> hyper_parameters;

	using PhysicsCore::PhysicsCore;
	using NeatCore::NeatCore;
	Actor();
	Actor(Ball& b);
	Actor(Ball& b, int s);
	Actor(const Genome& g);
	Actor(const Ball& b, const Genome& g);

	void set_inputs(std::vector<float> inputs) override;
	void act() override;
	Genome breed(NeatCore* other) override;
	double calculate_fitness() override;
	void normalize_fitness() override;
	void epoch() override;
	Actor* clone() override;
	int get_network_size() override;
	std::tuple<float, float, float> get_mismatch(std::shared_ptr<NeatCore> interloper) override;
	void update(float delta_time) override;
	virtual void NEAT_update();
	bool hit(sf::Vector2f source, float angle, float range, sf::Vector2f point, float rad) const;
	std::pair<int, float> hit(sf::Vector2f source, float angle, float range,
	                          const std::unordered_map<int, std::shared_ptr<Actor>>& actors) const;
	virtual void hold(std::shared_ptr<Actor> a);
	virtual void hold(std::shared_ptr<Actor> a, float strength);
	virtual void ray_cast(const std::unordered_map<int, std::shared_ptr<Actor>>& actors);

	
};

