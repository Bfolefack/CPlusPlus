#pragma once

struct Collision
{
public:
	bool collision = false;
	int ball1;
	int ball2;
	float dist;
	sf::Vector2f ball1_overlap;
	sf::Vector2f ball2_overlap;
};
