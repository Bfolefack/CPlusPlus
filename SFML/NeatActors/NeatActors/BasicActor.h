#pragma once
#include "Actor.h"

class Actor;
class BasicActor : public Actor
{
public:
	using Actor::Actor;

	BasicActor();
	BasicActor(Ball& b);
	void update(float delta_time) override;
	//~BasicActor() override = default;
};
