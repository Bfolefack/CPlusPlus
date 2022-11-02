#include "Actor.h"
#include "Ball.h"
#include "Ray.h"

Actor::Actor()
{
	for_deletion = false;
	ball = Ball(10, 0, 0, 10, .9f);
	id = -1;
	sprite_id = -1;
	facing = 0;
	sight_range = 100;
	mutex = std::make_shared<std::mutex>();
}

Actor::Actor(const Ball& b)
{
	for_deletion = false;
	ball = b;
	id = -1;
	sprite_id = -1;
	facing = 0;
	sight_range = 100;
	mutex = std::make_shared<std::mutex>();
}

void Actor::ray_cast(std::vector<std::shared_ptr<Actor>>& actors)
{
	int count = 0;
	for(const auto a : ray_angles)
	{
		const auto ind = Ray::hit(ball.pos, a + facing, sight_range, actors);
		if(ind == -1)
		{
			sighted_actors[count] = nullptr;
		} else
		{
			sighted_actors[count] = actors[ind];
		}
		count++;
	}
}


void Actor::setId(int i)
{
	id = i;
	ball.id = i;
}

int Actor::getId() const
{
	return id;
}

//sf::Vector2f Actor::get_pos() const
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	return ball.pos;
//}
//
//sf::Vector2f Actor::get_vel()
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	return ball.vel;
//}
//
//sf::Vector2f Actor::get_acc()
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	return ball.acc;
//}
//
//void Actor::set_pos(sf::Vector2f pos)
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	ball.pos = pos;
//}
//
//void Actor::set_vel(sf::Vector2f vel)
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	ball.vel = vel;
//}
//
//
//void Actor::set_acc(sf::Vector2f acc)
//{
//	std::lock_guard<std::mutex> lock(*mutex);
//	ball.acc = acc;
//}

