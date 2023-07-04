#pragma once
#include "Actor.h"

class Actor;
class VeganActor : public Actor
{
public:
	sf::Vector2f desired_vector;
	int turn_countdown;
	float wander_angle;
	float max_turn;
	float max_speed;
	float size;
	float child_energy_threshold;
	float child_starting_energy;
	float energy;
	float mutation_rate;

	using Actor::Actor;
	void loop();
	void hold(std::shared_ptr<Actor> a) override;
	/*FlockActor();*/
	void update(float delta_time) override;
	
	void mutate();
	float mutate(float num) const;
	
	static int rand_range(int min, int max);

	VeganActor(const Ball& b);
	VeganActor(const VeganActor& parent);
	//~BasicActor() override = default;
};
