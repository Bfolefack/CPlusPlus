#pragma once
#include "Actor.h"

class Actor;
class FlockActor : public Actor
{
public:
	float steer_angle = 0;
	using Actor::Actor;
	void loop();
	/*FlockActor();*/
	void update(float delta_time) override;
	FlockActor(const Ball& b);
	FlockActor(int w, int h);
	//~BasicActor() override = default;
};
