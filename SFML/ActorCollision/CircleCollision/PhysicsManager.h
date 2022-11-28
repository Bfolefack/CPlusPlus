#pragma once

#include <future>
#include <queue>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "CriticalMutex.h"
#include "ChunkInterface.h"
#include "Actor.h"

class PhysicsChunk;
struct CriticalMutex;

using std::shared_ptr;


class PhysicsManager
{
public:
	std::unordered_map<int, std::unique_ptr<CriticalMutex>> chunk_threads;
	std::unordered_map<int, shared_ptr<Actor>> actors;
	sf::Vector2f mousePos;
	shared_ptr<Actor> selectedActor;
	shared_ptr<Actor> velSelectedActor;
	int chunk_width;
	int chunk_height;
	int height;
	int width;
	int next_actor;
	static int global_selected_actor;

	std::vector<sf::VertexArray> vertex_arrays;
	std::vector<sf::Texture> textures;
	std::vector<std::unordered_set<int>> texture_groups;

	int MAX_THREADS = 12;
	std::unordered_map<int, int> chunks_to_threads;
	std::mutex mutex;

	PhysicsManager(int width, int height, int chunk_width, int chunk_height);
	static bool is_inside_box(sf::Vector2f pos, sf::Vector2f box_pos, sf::Vector2f box_size);
	void draw(sf::RenderWindow& window);
	void update();
	std::array<int, 4> place_actor(int b);
	void add_actor(std::vector<shared_ptr<Actor>> actors);
	void re_add_actor(std::unordered_set<int> added_actors);
	//void add_actor(Actor& actor);
	void re_add_actor(int id);
	void add_actor(shared_ptr<Actor> actor);
	static void threaded_add_actor(std::mutex& mutex, std::list<shared_ptr<Actor>>& limbo_list, shared_ptr<Actor> actor);
	int get_chunk(sf::Vector2f vec) const;
	void select_actor();
	void deselect_actor();
	void vel_select_actor();
	void vel_deselect_actor();
	void drawLine(sf::Vector2f point1, sf::Vector2f point2, sf::Color color, sf::RenderWindow& window);
	void thread_update();
	void perpetual_update();
};
