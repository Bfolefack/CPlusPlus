#pragma once
#include "Actor.h"

class Actor;
class BasicActor : public Actor
{
public:
	using Actor::Actor;

	BasicActor();
	BasicActor(Ball& b);
	void update() override;
	void draw() override;
	//~BasicActor() override = default;
};
