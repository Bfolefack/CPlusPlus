#pragma once

#include "Ball.h"
#include "SFML/Graphics.hpp"
#include <vector>
#include <queue>
#include <unordered_map>

using std::vector;
using std::unordered_map;
using std::shared_ptr;



class PhysicsChunk {
public:
	unordered_map<int, shared_ptr<Ball>>  balls;
	shared_ptr<Ball> selectedBall;
	shared_ptr<Ball> velSelectedBall;
	std::queue<Collision> collision_stack;
	sf::Vector2f mousePos;
	sf::Vector2f pos;
	sf::Vector2f size;
	std::list<int> exiting_balls;
	PhysicsChunk();
	PhysicsChunk(sf::Vector2f pos, sf::Vector2f size);
	std::list<int> update(float delta_time);
	void add_ball(int id, const Ball& ball);
	void add_ball(int id, const std::shared_ptr<Ball>& ball);
	void remove_ball(int id);
	void draw(sf::RenderWindow& window);
	void select_ball();
	void deselect_ball();
	auto vel_select_ball() -> void;
	void vel_deselect_ball();
	void resolve_collisions(float delta_time);
};