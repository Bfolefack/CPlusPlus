#pragma once
#include "Actor.h"

class PlantActor : public Actor
{
public:
	using Actor::Actor;
	static std::atomic<float> world_nutrition;
	float size;
	float projected_growth;
	int holder_size;
	PlantActor();
	PlantActor(Ball& b);
	PlantActor(Ball& b, int s);
	void update(float delta_time) override;
	void ray_cast(const std::unordered_map<int, std::shared_ptr<Actor>>& actors);
	//~BasicActor() override = default;
};
