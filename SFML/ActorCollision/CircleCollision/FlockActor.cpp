#include "FlockActor.h"

#include <iostream>

void FlockActor::draw() {}

FlockActor::FlockActor(int w, int h) : Actor()
{
	world_width = w;
	world_height = h;
	ball.always_active = true;
	for (float f = -3.14159265358979323846f; f < 3.14159265358979323846f; f += 3.14159265358979323846f / 5)
	{
		ray_angles.push_back(f);
		sighted_actors.push_back(nullptr);
	}
	ball.vel = { -2, 0 };
	facing = ((rand() % 6283)) / 1000.f;
	ball.rad = 25;
	//std::cout << facing << std::endl;
	sight_range = 100;
}

sf::Vector2f& FlockActor::avoid_walls()
{
	sf::Vector2f out = {0, 0};
	if(ball.pos.x < sight_range)
		out += {1, 0};
	if (ball.pos.x > world_width - sight_range)
		out += {-1, 0};
	if (ball.pos.y < sight_range)
		out += {0, 1};
	if (ball.pos.y > world_height - sight_range)
		out += {0, -1};
	return out;
}

float FlockActor::avoid_walls_angle()
{
	sf::Vector2f out = { 0, 0 };
	if (ball.pos.x < sight_range)
		out += {1, 0};
	if (ball.pos.x > world_width - sight_range)
		out += {-1, 0};
	if (ball.pos.y < sight_range)
		out += {0, 1};
	if (ball.pos.y > world_height - sight_range)
		out += {0, -1};
	return atan2f(out.y, out.x);
}

void FlockActor::update()
{
	//std::lock_guard<std::mutex> lck(*mutex);
	if(facing > 3.14159265358979323846f)
	{
		facing -= 2 * 3.14159265358979323846f;
	}

	if (facing < -3.14159265358979323846f)
	{
		facing += 2 * 3.14159265358979323846f;
	}

	sf::Vector2f pos_avg = { 0, 0 };
	int count = 0;

	for (auto& a : sighted_actors)
	{
		if(a != nullptr)
		{
			pos_avg += a->ball.pos;
			count++;
		}
	}
	if (count > 0) {
		pos_avg /= (float)count;
		pos_avg = (ball.pos - pos_avg) / 100.f;
		facing += 0.1f * ((atan2(pos_avg.y, pos_avg.x) + avoid_walls_angle()) / 2.f);
	}
	if(rand() % 1000 < 2)
	{
		//for_deletion = true;
	}
	ball.acc = sf::Vector2f( sin(facing), cos(facing) );
}
