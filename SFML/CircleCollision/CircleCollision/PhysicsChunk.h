#pragma once

#include "Collision.h"
#include "Border.h"
#include "SFML/Graphics.hpp"

#include <mutex>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "CriticalMutex.h"

struct Border;
struct Collision;

class PhysicsManager;
struct ChunkInterface;
class Ball;

using std::vector;
using std::unordered_map;
using std::shared_ptr;

struct CriticalMutex;

class PhysicsChunk
{
public:
	static std::unordered_map <int, shared_ptr<PhysicsChunk>> chunk_map;
	static std::mutex chunk_map_mutex;

	int id;
	unordered_map<int, shared_ptr<Ball>> balls;
	std::queue<Collision> collision_stack;
	sf::Vector2f pos;
	sf::Vector2f size;
	std::list<int> exiting_balls;
	Border bounds;
	bool is_border = false;

	PhysicsChunk();
	PhysicsChunk(int i, sf::Vector2f p, sf::Vector2f s, Border b);
	static void compounded_update(std::unique_ptr<CriticalMutex>& mutex);
	void update(float deltaTime, ChunkInterface& ct);
	void perpetual_update(float deltaTime, ChunkInterface& ct, std::mutex& m);
	static void update(shared_ptr<PhysicsChunk> pc, float deltaTime,
	                   shared_ptr<std::unordered_set<int>>& exiting_balls);
	void add_ball(int id, const Ball& ball);
	void add_ball(int id, const shared_ptr<Ball>& ball);
	void remove_ball(int id);
	void draw(sf::RenderWindow& window);
	bool isInside(sf::Vector2f pos);
	//void select_ball(sf::Vector2f mouse_pos);
	//void deselect_ball(sf::Vector2f mouse_pos);
	//auto vel_select_ball(sf::Vector2f mouse_pos) -> void;
	//void vel_deselect_ball(sf::Vector2f mouse_pos);
	void resolve_collisions(float delta_time);
private:
	std::queue<int> physics_update(float deltaTime);
};

