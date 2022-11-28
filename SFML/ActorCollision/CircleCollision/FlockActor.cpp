#include "FlockActor.h"

#include <iostream>

void FlockActor::draw() {}

FlockActor::FlockActor(const Ball& b) : Actor(b)
{
	ball = b;
	ball.active = true;
	ball.always_active = true;
	ball.friction = 0.025f;
	for (float f = -3.14159265358979323846f; f < 3.14159265358979323846f; f += 3.14159265358979323846f / 7)
	{
		ray_angles.push_back(f);
		sighted_actors.push_back(nullptr);
	}

	facing = ((rand() % 6284) - 3142) / 1000.f;
	ball.vel = { cos(facing), sin(facing)};
	ball.vel *= 0.05f;
	ball.rad = 15;

	actor_type = rand() % 5;

	if (actor_type == 0)
	{
		sprite_id = 0;
		sprite_color = sf::Color::Red;
	}
	else if (actor_type == 1)
	{
		sprite_id = 0;
		sprite_color = sf::Color::Green;
	}
	else if (actor_type == 2)
	{
		sprite_id = 0;
		sprite_color = sf::Color::Blue;
	}
	else if (actor_type == 3)
	{
		sprite_id = 0;
		sprite_color = sf::Color::Yellow;
	}
	else if (actor_type == 4)
	{
		sprite_id = 0;
		sprite_color = sf::Color::Magenta;
	}
	//std::cout << facing << std::endl;
	sprite_id = 0;
	sight_range = 200;
}

void FlockActor::loop()
{
	if (ball.pos.x < ball.rad * 2)
	{
		ball.pos.x = world_size.x - 1;
	}
	if (ball.pos.y < ball.rad * 2)
	{
		ball.pos.y = world_size.y - 1;
	}
	if (ball.pos.x > world_size.x - ball.rad * 2)
	{
		ball.pos.x = 1;
	}
	if (ball.pos.y > world_size.y - ball.rad * 2)
	{
		ball.pos.y = 1;
	}
}

void FlockActor::update()
{
	sf::Vector2f avoid = { 0, 0 };
	std::lock_guard<std::mutex> lck(*mutex);
	sf::Vector2f vel_sum = { 0, 0 };
	
	bool kill = true;
	for (const auto& a : sighted_actors)
	{
		if (a != nullptr) {
			kill = false;
			auto vec = a->ball.pos - ball.pos;
			float dist = Ball::magsq(vec);
			if (actor_type == a->actor_type) {
				if (dist < 30 * 30)
				{
					avoid -= ((vec) * (1 - sqrt(dist) / sight_range));
				}
				else if (dist < 100 * 100 && dist > 45 * 45)
				{
					avoid += ((vec) * (1 - sqrt(dist) / sight_range)) * 5.f;
				}
			} else
			{
				avoid -= ((vec) * (1 - sqrt(dist) / sight_range));
			}
			vel_sum += a->ball.vel * (1 - sqrt(dist) / sight_range);
		}
		//std::cout << sqrt(Ball::magsq(avoid)) << std::endl;
	}
	//if (kill)
	//{
	//	for_deletion = true;
	//}
	if(avoid.x != 0 && avoid.y != 0)
	{
		avoid /= sqrt(Ball::magsq(avoid));
		avoid *= 0.05f;
	}
	if (vel_sum.x != 0 && vel_sum.y != 0)
	{
		vel_sum /= sqrt(Ball::magsq(vel_sum));
		vel_sum *= .05f;
	}

	auto steer = avoid + vel_sum + avoid_walls() * 0.05f;
	if (steer.x != 0 && steer.y != 0)
	{
		steer /= sqrt(Ball::magsq(steer));
		steer *= 0.05f;
	} else
	{
		steer = { cos(facing), sin(facing) };
	}
	ball.acc += steer;
	if(Ball::magsq(ball.acc) > 0.01)
	{
		ball.acc /= sqrt(Ball::magsq(ball.acc)) * 100;
	}

	facing = atan2(ball.vel.y, ball.vel.x);
	//loop();
	//ball.acc = { 1, 0 };
}
