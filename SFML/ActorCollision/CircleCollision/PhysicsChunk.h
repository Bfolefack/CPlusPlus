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
class Actor;

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
	unordered_map<int, shared_ptr<Actor>> actors;
	std::queue<Collision> collision_stack;
	sf::Vector2f pos;
	sf::Vector2f size;
	std::list<int> exiting_actors;
	Border bounds;
	bool is_border = false;
	bool active = true;

	float ray_update_buffer = 0;

	PhysicsChunk();
	PhysicsChunk(int i, sf::Vector2f p, sf::Vector2f s, Border b);
	static std::array<int, 4> place_actor(const shared_ptr<Actor>& actor, std::unordered_map<int, int>& chunk_ids, int chunk_size_x, int chunk_size_y, std::vector<
	                                     std::shared_ptr<PhysicsChunk>>& chunks, bool bulk);
	static void compounded_update(std::unique_ptr<CriticalMutex>& mutex);
	void update(float deltaTime, ChunkInterface& ct);
	void perpetual_update(float deltaTime, ChunkInterface& ct, std::mutex& m);
	static void update(shared_ptr<PhysicsChunk> pc, float deltaTime,
	                   shared_ptr<std::unordered_set<int>>& exiting_actors);
	//void add_actor(int id, const Actor& actor);
	void add_actor(int id, const shared_ptr<Actor>& actor);
	void add_actor(const shared_ptr<Actor>& actor);
	void add_actor(const std::list<shared_ptr<Actor>>& actor);
	void remove_actor(int id);
	void draw(sf::RenderWindow& window);
	bool isInside(sf::Vector2f pos);
	//void select_actor(sf::Vector2f mouse_pos);
	//void deselect_actor(sf::Vector2f mouse_pos);
	//auto vel_select_actor(sf::Vector2f mouse_pos) -> void;
	//void vel_deselect_actor(sf::Vector2f mouse_pos);
	void resolve_collisions(float delta_time);
private:
	std::pair<std::queue<int>, std::queue<int>> physics_update(float delta_time);
};

