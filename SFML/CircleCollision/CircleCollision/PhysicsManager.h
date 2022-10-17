#pragma once

#include <future>
#include <queue>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "CriticalMutex.h"
#include "ChunkInterface.h"
#include "Ball.h"

class PhysicsChunk;
struct CriticalMutex;

using std::shared_ptr;


class PhysicsManager
{
public:
	std::unordered_map<int, std::unique_ptr<CriticalMutex>> chunk_threads;
	std::unordered_map<int, shared_ptr<Ball>> balls;
	sf::Vector2f mousePos;
	shared_ptr<Ball> selectedBall;
	shared_ptr<Ball> velSelectedBall;
	int chunk_width;
	int chunk_height;
	int height;
	int width;
	int next_ball;

	sf::VertexArray circles;
	sf::Texture circleTexture;
	int MAX_THREADS = 1;
	std::unordered_map<int, int> chunks_to_threads;
	std::mutex mutex;

	PhysicsManager(int width, int height, int chunkWidth, int chunkHeight);
	static bool is_inside_box(sf::Vector2f pos, sf::Vector2f box_pos, sf::Vector2f box_size);
	void perpetual_draw(sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);
	void update();
	std::array<int, 4> place_ball(int b);
	void add_ball(std::vector<shared_ptr<Ball>> balls);
	void re_add_ball(std::unordered_set<int> added_balls);
	void add_ball(Ball ball);
	void re_add_ball(int id);
	void add_ball(shared_ptr<Ball> ball);
	static void threaded_add_ball(std::mutex& mutex, std::list<shared_ptr<Ball>>& limbo_list, shared_ptr<Ball> ball);
	int get_chunk(sf::Vector2f vec) const;
	void select_ball();
	void deselect_ball();
	void vel_select_ball();
	void vel_deselect_ball();
	void drawLine(sf::Vector2f point1, sf::Vector2f point2, sf::Color color, sf::RenderWindow& window);
	void thread_update();
	void perpetual_update();
};
