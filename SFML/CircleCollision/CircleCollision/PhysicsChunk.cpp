
#include "PhysicsChunk.h"
#include "Ball.h"
#include "Collision.h"
#include "ChunkInterface.h"
#include "PhysicsManager.h"
#include "Border.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <deque>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <Windows.h>


using std::vector;
using std::queue;
using std::shared_ptr;
using std::unordered_map;

std::unordered_map<int, shared_ptr<PhysicsChunk>> PhysicsChunk::chunk_map = std::unordered_map<int, shared_ptr<PhysicsChunk>>();
std::mutex PhysicsChunk::chunk_map_mutex;

PhysicsChunk::PhysicsChunk()
{
	size = sf::Vector2f(1900, 1000);
	balls = unordered_map<int, shared_ptr<Ball>>();
	exiting_balls = std::list<int>();
	collision_stack = queue<Collision>();
}

PhysicsChunk::PhysicsChunk(int i, sf::Vector2f p, sf::Vector2f s, Border b)
{
	id = i;
	pos = p;
	size = s;
	balls = unordered_map<int, shared_ptr<Ball>>();
	exiting_balls = std::list<int>();
	collision_stack = queue<Collision>();
	bounds = b;
	if (b.left || b.right || b.top || b.bottom)
		is_border = true;
}

void PhysicsChunk::compounded_update(std::unique_ptr<CriticalMutex>& mutex)
{
	
	std::vector<shared_ptr<PhysicsChunk>> chunks;
	while(CriticalMutex::all_frozen)
	{
		Sleep(1);
	}
	{
		
		//std::cout << "populating exiting balls: " <<  << std::endl;
		std::lock_guard<std::mutex> lock(chunk_map_mutex);
		for (auto i : mutex->chunk_ids)
		{
			chunks.push_back(chunk_map[i]);
		}
	}
	float delta_time = 1;
	while (true)
	{
		auto start = std::chrono::high_resolution_clock::now();
		if (!mutex->all_frozen) {
			if(!mutex->limbo_list.empty())
			{
				for(auto i : mutex->limbo_list)
				{
					mutex->chunk_interface[i.first]->limbo_list.splice(mutex->chunk_interface[i.first]->limbo_list.end(), i.second);
				}
			}
			std::unique_lock<std::mutex> lock(*mutex->mutex, std::defer_lock);
			if (lock.try_lock()) {
				for (auto& i : chunks)
				{
					i->update(delta_time, *mutex->chunk_interface[i->id]);
				}
			}
			for (auto& i : mutex->chunk_interface)
			{
				mutex->exiting_balls.insert(i.second->exiting_balls.begin(), i.second->exiting_balls.end());
			}
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto runtime = (float) std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		delta_time = runtime / 6944.4;

	}
}

void PhysicsChunk::update(float deltaTime, ChunkInterface& ct)
{
	if (!CriticalMutex::all_frozen) {
			if (!balls.empty()) {
				auto exit = physics_update(deltaTime);
				//std::cout << pos.x << ", " << pos.y << std::endl;
				while (!exit.empty())
				{
					auto id = exit.front();
					//std::cout << "exiting ball: " << id << std::endl;
					balls.erase(id);
					exiting_balls.push_back(id);
					exit.pop();
				}
				for (shared_ptr<Ball> b : ct.limbo_list)
				{
					add_ball(b->id, b);
				}
				for (auto b : exiting_balls)
				{
					ct.exiting_balls.insert(b);
				}
				exiting_balls.clear();
			}


			if (!ct.limbo_list.empty())
			{
				for (auto i : ct.limbo_list)
				{
					add_ball(i->id, i);
				}
				ct.limbo_list.clear();
			}
			//std::cout << "Chunk Update: " << deltaTime << " microseconds" << "(" << balls.size() << " Balls)" << std::endl;
		}
}

void PhysicsChunk::perpetual_update(float deltaTime, ChunkInterface& ct, std::mutex& m)
{
	while (true) {
		Sleep(1);
		update(deltaTime, ct);
	}
}


std::queue<int> PhysicsChunk::physics_update(float deltaTime)
{
	if (balls.empty())
		return {};
	
	std::deque<shared_ptr<Ball>> temp_balls;
	
	for (auto b : balls) {
		temp_balls.push_back(b.second);
	}
	collision_stack = queue<Collision>();
	for (const auto b1 : balls) {
		//std::cout << "[" << pos.x/(size.x/2) << ", " << pos.y/(size.y/2) << "]" << b1.first << std::endl;
		//temp_balls.pop_front();
		for (const auto b2 : temp_balls) {
			auto coll = Ball::collide(b1.second, b2);
			if (coll.collision)
				collision_stack.push(coll);
		}
	}

	/*resolve collisions*/ {
		const auto temp = collision_stack;
		while (!collision_stack.empty()) {


			auto collision = collision_stack.front();
			collision_stack.pop();
			const auto b1 = balls[collision.ball1];
			const auto b2 = balls[collision.ball2];

			std::lock(*b1->mutex, *b2->mutex);
			std::lock_guard<std::mutex> lck1(*b1->mutex, std::adopt_lock);
			std::lock_guard<std::mutex> lck2(*b2->mutex, std::adopt_lock);
			//if (std::try_lock(b1_lock, b2_lock)) {
				//auto start = std::chrono::high_resolution_clock::now();
				///std::cout << "locked b1 & b2" << std::endl;
				//Complex Maths
			const auto normal = -(b2->pos - b1->pos) / sqrt(Ball::magsq((b2->pos - b1->pos)));
			const auto tangent = sf::Vector2f(-normal.y, normal.x);


			const auto tan_dp1 = (b1->vel.x) * tangent.x + (b1->vel.y) * tangent.y;
			const auto tan_dp2 = (b2->vel.x) * tangent.x + (b2->vel.y) * tangent.y;

			const auto norm_dp1 = (b1->vel.x) * normal.x + (b1->vel.y) * normal.y;
			const auto norm_dp2 = (b2->vel.x) * normal.x + (b2->vel.y) * normal.y;

			////Conservation of momentum
			const auto m1 = (norm_dp1 * (b1->mass - b2->mass) + 2.f * b2->mass * norm_dp2) / (b1->mass + b2->mass);
			const auto m2 = (norm_dp2 * (b2->mass - b1->mass) + 2.f * b1->mass * norm_dp1) / (b1->mass + b2->mass);

			//std::cout << 1 << ": " << std::this_thread::get_id() << std::endl;
			b1->collisionStack.insert({ b2->id, {(deltaTime < 1 ? deltaTime : 1), collision.ball1_overlap, (((tangent * tan_dp1) + (normal * m1)) * (b1->elasticity * b2->elasticity))} });
			//std::cout << 2 << ": " << std::this_thread::get_id() << std::endl;
			b2->collisionStack.insert({ b1->id, {(deltaTime < 1 ? deltaTime : 1), collision.ball2_overlap, (((tangent * tan_dp2) + (normal * m2)) * (b1->elasticity * b2->elasticity))} });
			//auto end = std::chrono::high_resolution_clock::now();
			//std::cout << "Double lock time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
		//} else
		//{
		//	//std::cout << "Ball Lock Blocked" << std::endl;
		//}
		}
		collision_stack = temp;


		if (is_border)
			for (auto b1 : balls) {
				if (b1.second->pos.x + b1.second->rad > (pos.x + size.x) && bounds.right) {
					b1.second->vel.x *= -1 * b1.second->elasticity;
					b1.second->pos.x = (pos.x + size.x) - b1.second->rad;
					b1.second->collision = true;
				}
				if (b1.second->pos.x - b1.second->rad < (pos.x) && bounds.left) {
					b1.second->vel.x *= -1 * b1.second->elasticity;
					b1.second->pos.x = (pos.x) + b1.second->rad;
					b1.second->collision = true;
				}
				if (b1.second->pos.y + b1.second->rad > (pos.y + size.y) && bounds.bottom) {
					b1.second->vel.y *= -1 * b1.second->elasticity;
					b1.second->pos.y = (pos.y + size.y) - b1.second->rad;
					b1.second->collision = true;
				}
				if (b1.second->pos.y - b1.second->rad < (pos.y) && bounds.top) {
					b1.second->vel.y *= -1 * b1.second->elasticity;
					b1.second->pos.y = (pos.y) + b1.second->rad;
					b1.second->collision = true;
				}
			}
		//std::cout << bounds.left << ", " << bounds.right << ", " << bounds.top << ", " << bounds.bottom << std::endl;

		std::queue<int> temp_exit;
		for (auto b : balls) {
			std::unique_lock <std::mutex > b_lock(*b.second->mutex);
			b.second->update();
			if (((b.second->pos.x < pos.x) && !bounds.left) || ((b.second->pos.x > pos.x + size.x) && !bounds.right) || ((b.second->pos.y < pos.y) && !bounds.top) || ((b.second->pos.y > pos.y + size.y) && !bounds.bottom))
			{
				temp_exit.push(b.first);
				//remove_ball(b.first);
			}
		}
		return temp_exit;
	}
	return {};
	
	
}



void PhysicsChunk::draw( sf::RenderWindow& window)
{
	
	for (auto b : balls) {
		b.second->draw(window);
	}
	
	/*for (int i = 0; i < collision_stack.size(); i++) {
		Collision temp = collision_stack.front();
		collision_stack.pop();
		drawLine(balls[temp.ball1]->pos, balls[temp.ball2]->pos, sf::Color::Red, window);
	}*/
}

bool PhysicsChunk::isInside(const sf::Vector2f obj_pos)
{
	return (obj_pos.x > pos.x && obj_pos.x < pos.x + size.x && obj_pos.y > pos.y && obj_pos.y < pos.y + size.y);
}

void PhysicsChunk::add_ball(int id, const Ball& ball)
{
	balls.insert({id, std::make_shared<Ball>(ball) });
}

void PhysicsChunk::add_ball(int id, const shared_ptr<Ball>& ball)
{
	balls.insert({ id, ball });
}


