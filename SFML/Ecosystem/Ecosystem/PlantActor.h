#pragma once
#include "Actor.h"

class Actor;
class PlantActor : public Actor
{
public:
	using Actor::Actor;
	static std::atomic<float> world_nutrition;
	static constexpr int nutrition_map_default = 2000;
	static constexpr int nutrition_map_size = 2000;
	float projected_growth;
	int growth_rate;
	int max_growth;
	int child_num;
	int max_growth_step;
	float ejection_force;
	float elasticity;
	float friction;
	float mutation_rate;
	float size;
	int age;
	int max_age;


	PlantActor();
	PlantActor(int gr, int mg, int cn, float ef, float elas, float fric, float mr, int mgs, int ma);
	void mutate();
	PlantActor(Ball& b);
	void update(float delta_time) override;
	void ray_cast(const std::unordered_map<int, std::shared_ptr<Actor>>& actors);
	//~BasicActor() override = default;
};
