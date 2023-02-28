#pragma once
#include "Actor.h"

class Actor;
class PlantActor : public Actor
{
public:
	using Actor::Actor;
	static std::unordered_map<int, std::unordered_map<int, int>> nutrition_map;
	static std::mutex nutrition_map_mutex;
	static constexpr int nutrition_map_default = 200;
	static constexpr int nutrition_map_size = 2000;
	int growth_rate;
	int max_growth;
	int child_num;
	int max_growth_step;
	float ejection_force;
	float elasticity;
	float friction;
	float mutation_rate;
	int size;
	int age;
	int max_age;


	PlantActor();
	PlantActor(int gr, int mg, int cn, float ef, float elas, float fric, float mr, int mgs, int ma);
	PlantActor(Ball& b);
	void update() override;
	void draw() override;
	void ray_cast(const std::unordered_map<int, std::shared_ptr<Actor>>& actors);
	//~BasicActor() override = default;
};
