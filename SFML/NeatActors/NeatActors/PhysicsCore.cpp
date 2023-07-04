#include "PhysicsCore.h"

#include <iostream>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "Ball.h"

sf::Vector2i PhysicsCore::world_size = { 0, 0 };

PhysicsCore::PhysicsCore()
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

PhysicsCore::PhysicsCore(const Ball& b)
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

sf::Vector2f PhysicsCore::avoid_walls() const
{
	sf::Vector2f out = { 0, 0 };
	if (ball.pos.x < sight_range)
		out += {1, 0.0001};
	if (ball.pos.x > world_size.x - sight_range)
		out += {-1, 0.0001};
	if (ball.pos.y < sight_range)
		out += {0.0001, 1};
	if (ball.pos.y > world_size.y - sight_range)
		out += {0.0001, -1};
	return out;
}



void PhysicsCore::setId(int i)
{
	id = i;
	ball.id = i;
}

int PhysicsCore::getId() const
{
	return id;
}





//sf::Vector2f PhysicsCore::get_pos() const
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	return ball.pos;
//}
//
//sf::Vector2f PhysicsCore::get_vel()
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	return ball.vel;
//}
//
//sf::Vector2f PhysicsCore::get_acc()
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	return ball.acc;
//}
//
//void PhysicsCore::set_pos(sf::Vector2f pos)
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	ball.pos = pos;
//}
//
//void PhysicsCore::set_vel(sf::Vector2f vel)
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	ball.vel = vel;
//}
//
//
//void PhysicsCore::set_acc(sf::Vector2f acc)
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	ball.acc = acc;
//}

