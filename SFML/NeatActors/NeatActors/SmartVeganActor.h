#pragma once
#include "Actor.h"
class SmartVeganActor :
    public Actor
{
public:
	float agility;
	float max_speed;
	float size;
	float child_starting_energy;
	//float sight_range;
	float fov = 0.5;
	float bite_cooldown;
	int Red, Green, Blue;


	float energy;
	float mutation_rate;
	std::shared_ptr<Actor> closest_actor;
	float sq_closest_actor_dist;
	float lifetime_energy_gain;

	using Actor::Actor;
	void hold(std::shared_ptr<Actor> a, float strength) override;
	void bite(float delta_time, std::shared_ptr<Actor> closest_actor, float sq_closest_actor_dist,
	          std::vector<float> outputs);
	void NEAT_update() override;
	void update(float delta_time) override;

	void mutate();
	float mutate(float num) const;
	static float rand_range(int min, int max);
	void ray_cast(const std::unordered_map<int, std::shared_ptr<Actor>>& actors) override;
	void initialize_sight_rays();

	SmartVeganActor(const Ball& b);
	SmartVeganActor(const SmartVeganActor& parent);
};

