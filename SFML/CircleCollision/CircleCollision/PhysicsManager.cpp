#include "PhysicsManager.h"
#include <iostream>

PhysicsManager::PhysicsManager(int w, int h, int cw, int ch)
{
	width = w;
	height = h;
	chunk_width = cw;
	chunk_height = ch;
	map = unordered_map<int, std::shared_ptr<PhysicsChunk>>();
	for (int i = 0; i < width / cw; ++i)
	{
		for (int j = 0; j < height / ch; ++j)
		{
			map.insert({ i + j * 100000, std::make_shared<PhysicsChunk>(PhysicsChunk(sf::Vector2f(i * cw, j * ch), sf::Vector2f(cw, ch))) });
		}
	}
}

void PhysicsManager::draw(sf::RenderWindow& window) const
{
	for (auto p : map)
	{
		p.second->draw(window);
	}
}
void PhysicsManager::update()
{
	auto exiting_balls = std::list<int>();
	
	for (auto p : map)
	{
		p.second->update(1);
		exiting_balls.splice(exiting_balls.begin(), p.second->update(1));
	}
	for(const int i : exiting_balls)
	{
		place_ball(i);
	}
}

void PhysicsManager::place_ball(const int b)
{
	const auto ball = balls[b];
	const auto chunk_x = static_cast<int>(ball->pos.x) / chunk_width;
	const auto chunk_y = static_cast<int>(ball->pos.y) / chunk_height;
	const int chunk = chunk_x + (chunk_y * 100000);
	if(map.count(chunk) <= 0)
	{
		balls.erase(b);
		return;
	}
	map[chunk]->add_ball(b, ball);
	if(chunk_x > 0 && chunk_y > 0)
	{
		map[chunk - 1]->add_ball(b, ball);
		map[chunk - 100000]->add_ball(b, ball);
		map[chunk - 100001]->add_ball(b, ball);
	} else if(chunk_x > 0)
	{
		map[chunk - 1]->add_ball(b, ball);
	} else if(chunk_y > 0)
	{
		map[chunk - 100000]->add_ball(b, ball);
	}
}

void PhysicsManager::add_ball(Ball ball)
{
	add_ball(std::make_shared<Ball>(ball));
}

void PhysicsManager::add_ball(shared_ptr<Ball> ball)
{
	std::cout << balls.size() << "\n";
	ball->id = static_cast<int>(balls.size());
	balls[static_cast<int>(balls.size())] = ball;
	place_ball(ball->id);
}


