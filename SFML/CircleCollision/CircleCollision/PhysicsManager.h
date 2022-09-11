#pragma once
#include <unordered_map>
#include "PhysicsChunk.h"
class PhysicsManager
{
public:
	std::unordered_map<int, shared_ptr<Ball>> balls;
	std::unordered_map <int, std::shared_ptr<PhysicsChunk>> map;
	int chunk_width;
	int chunk_height;
	int height;
	int width;

	PhysicsManager(int width, int height, int chunkWidth, int chunkHeight);
	void update();
	void place_ball(int b);
	void add_ball(Ball ball);
	void add_ball(shared_ptr<Ball> ball);
	void draw(sf::RenderWindow& window) const;
};