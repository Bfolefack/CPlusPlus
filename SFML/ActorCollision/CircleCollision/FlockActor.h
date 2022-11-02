#pragma once
#include "Actor.h"

class Actor;
class FlockActor : public Actor
{
public:
	using Actor::Actor;

	int world_width;
	int world_height;

	sf::Vector2f& avoid_walls();
	float avoid_walls_angle();
	/*FlockActor();*/
	void update() override;
	void draw() override;
	FlockActor(int w, int h);
	//~BasicActor() override = default;
};
