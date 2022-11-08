#include "BasicActor.h"

#include <utility>

void BasicActor::draw(){}

BasicActor::BasicActor() : Actor()
{

	ball.always_active = true;
	for (float f = -3.14159265358979323846f; f < 3.14159265358979323846f; f += 3.14159265358979323846f/5)
	{
		ray_angles.push_back(f);
		sighted_actors.push_back(nullptr);
	}
	sight_range = 300;
}

BasicActor::BasicActor(Ball& b) : Actor()
{
	ball = b;
	ball.always_active = true;
	for (float f = -3.14159265358979323846f; f < 3.14159265358979323846f; f += 3.14159265358979323846f / 5)
	{
		ray_angles.push_back(f);
		sighted_actors.push_back(nullptr);
	}
	sight_range = 300;
}

void BasicActor::update()
{
	
	sf::Vector2f avoid = { 0, 0 };
	std::lock_guard<std::mutex> lck(*mutex);
	for(const auto& a : sighted_actors)
	{
		if (a != nullptr)
		{
			
			sf::Vector2f vec = a->ball.pos - ball.pos;
			avoid += (vec) / sqrt(Ball::magsq(vec)) * 0.01f;
		}
	}
	ball.acc = avoid;
}
