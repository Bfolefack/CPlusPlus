#pragma once

#include <mutex>

#include "SFML/Graphics.hpp"
#include <unordered_map>

struct Collision;

class Ball {
public:
	int id;
	std::unordered_map<int, std::tuple<float, sf::Vector2<float>, sf::Vector2<float>>> collisionStack;
	sf::Vector2f pos;
	sf::Vector2f vel;
	sf::Vector2f acc;
	float rad;
	float mass;
	float elasticity;
	float delta_time = 1;
	static constexpr float max_vel = 400;
	static constexpr float sqrt_max_vel = 20;
	bool collision;
	bool active;
	bool always_active;

	//Ball(const Ball&) = delete;
	//Ball& operator= (const Ball&) = delete;
	
	Ball();
	Ball(int i, float r, float x, float y, float vx, float vy, float m, float e);
	Ball(float r, float x, float y, float m, float e);

	void update();
	void draw(sf::RenderWindow& window) const;
	bool isInside(sf::Vector2f v) const;

	static bool circlesOverlap(sf::Vector2f pos1, float rad1, sf::Vector2f pos2, float rad2);
	static Collision collide(Ball& ball1, Ball& ball2);
	//static sf::Vector2f closestPointOnLine(sf::Vector2f l1, sf::Vector2f l2, sf::Vector2f p);
	static float magsq(sf::Vector2f v);
};