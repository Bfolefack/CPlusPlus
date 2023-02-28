#include "PhysicsActor.h"

#include <iostream>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "Ball.h"

sf::Vector2i PhysicsActor::world_size = { 0, 0 };

PhysicsActor::PhysicsActor()
{
	for_deletion = false;
	ball = Ball(10, 0, 0, 10, .9f, 0.03f);
	id = -1;
	sprite_id = -1;
	sprite_color = sf::Color::White;
	actor_type = -1;
	facing = 0;
	sight_range = 0;
	mutex = std::make_shared<std::mutex>();
}

PhysicsActor::PhysicsActor(const Ball& b)
{
	for_deletion = false;
	ball = b;
	id = -1;
	sprite_id = -1;
	sprite_color = sf::Color::White;
	actor_type = -1;
	facing = 0;
	sight_range = 100;
	mutex = std::make_shared<std::mutex>();
}

sf::Vector2f PhysicsActor::avoid_walls() const
{
	sf::Vector2f out = { 0, 0 };
	if (ball.pos.x < sight_range)
		out += {1, 0};
	if (ball.pos.x > world_size.x - sight_range)
		out += {-1, 0};
	if (ball.pos.y < sight_range)
		out += {0, 1};
	if (ball.pos.y > world_size.y - sight_range)
		out += {0, -1};
	return out;
}

void PhysicsActor::ray_cast(const std::unordered_map<int, std::shared_ptr<PhysicsActor>>& actors)
{
	//std::lock_guard<std::mutex> lck(*mutex);
	int count = 0;
	for(const auto a : ray_angles)
	{
		const auto ind = hit(ball.pos, a + facing, sight_range, actors);
		if(ind == -1)
		{
			sighted_actors[count] = nullptr;
		} else
		{
			sighted_actors[count] = actors.at(ind);
		}
		count++;
	}
	//if(actors.size() > 30)
	//{
	//	std::cout << "ray_cast: " << actors.size() << std::endl;
	//}
}


void PhysicsActor::setId(int i)
{
	id = i;
	ball.id = i;
}

int PhysicsActor::getId() const
{
	return id;
}

bool PhysicsActor::hit(sf::Vector2f source, float angle, float range, sf::Vector2f point, float rad) const
{
	auto x1 = source.x - point.x;
	auto y1 = source.y - point.y;
	float x2 = x1 + range * cos(angle);
	float y2 = y1 + range * sin(angle);
	auto dr_sq = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	auto D = x1 * y2 - x2 * y1;
	float hit = rad * rad * dr_sq - D * D;
	return hit >= 0;
	//auto dr_sq = 
}

int PhysicsActor::hit(sf::Vector2f source, float angle, float range, const std::unordered_map<int, std::shared_ptr<PhysicsActor>>& actors) const
{
	float dist = range * range;
	int out = -1;
	for (const auto& a : actors)
	{
		
		if (a.first == id)
			continue;
		if (hit(source, angle, range, a.second->ball.pos, a.second->ball.rad))
		{
			const auto d = (source.x - a.second->ball.pos.x) * (source.x - a.second->ball.pos.x) + (source.y - a.second->ball.pos.y) * (source.y - a.second->ball.pos.y);
			if (d < dist)
			{
				out = a.first;
				dist = d;
			}
		}
		//casted.insert(a.first);
	}
	return out;
}

//sf::Vector2f PhysicsActor::get_pos() const
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	return ball.pos;
//}
//
//sf::Vector2f PhysicsActor::get_vel()
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	return ball.vel;
//}
//
//sf::Vector2f PhysicsActor::get_acc()
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	return ball.acc;
//}
//
//void PhysicsActor::set_pos(sf::Vector2f pos)
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	ball.pos = pos;
//}
//
//void PhysicsActor::set_vel(sf::Vector2f vel)
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	ball.vel = vel;
//}
//
//
//void PhysicsActor::set_acc(sf::Vector2f acc)
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	ball.acc = acc;
//}

