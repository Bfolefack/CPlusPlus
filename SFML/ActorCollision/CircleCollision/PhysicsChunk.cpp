
#include "PhysicsChunk.h"

#include <array>

#include "Actor.h"
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
	actors = unordered_map<int, shared_ptr<Actor>>();
	exiting_actors = std::list<int>();
	collision_stack = queue<Collision>();
}

PhysicsChunk::PhysicsChunk(int i, sf::Vector2f p, sf::Vector2f s, Border b)
{
	id = i;
	pos = p;
	size = s;
	actors = unordered_map<int, shared_ptr<Actor>>();
	exiting_actors = std::list<int>();
	collision_stack = queue<Collision>();
	bounds = b;
	if (b.left || b.right || b.top || b.bottom)
		is_border = true;
}

std::array<int, 4> PhysicsChunk::place_actor(const shared_ptr<Actor>& actor, std::unordered_map<int, int>& chunk_ids, int chunk_size_x, int chunk_size_y, std::vector <std::shared_ptr<PhysicsChunk>>& chunks,  bool bulk)
{
	const auto chunk_x = static_cast<int>((actor->ball.pos.x) / chunk_size_x);
	const auto chunk_y = static_cast<int>((actor->ball.pos.y) / chunk_size_y);
	const int chunk = chunk_x + (chunk_y * 100000);
	//chunk {0 0, 0 -1, -1 0, -1 -1}
	auto out = std::array<int, 4>({ -1, -1, -1, -1 });
	////map[chunk]->add_actor(b, actor);
	if(bulk)
	{
		if (chunk_ids.count(chunk) > 0)
		{
			out[0] = chunk;
			//chunks[chunk_ids[chunk]]->add_actor(actor);
		}
		if (chunk_ids.count(chunk - 1) > 0)
		{
			out[1] = chunk - 1;
			//chunks[chunk_ids[chunk - 1]]->add_actor(actor);
		}
		if (chunk_ids.count(chunk - 100000) > 0)
		{
			out[2] = chunk - 100000;
			//chunks[chunk_ids[chunk - 100000]]->add_actor(actor);
		}
		if (chunk_ids.count(chunk - 100001) > 0)
		{
			out[3] = chunk - 100001;
			//chunks[chunk_ids[chunk - 100001]]->add_actor(actor);
		}
	} else
	{
		if (chunk_ids.count(chunk) > 0)
		{
			out[0] = chunk;
			chunks[chunk_ids[chunk]]->add_actor(actor);
		}
		if (chunk_ids.count(chunk - 1) > 0)
		{
			out[1] = chunk - 1;
			chunks[chunk_ids[chunk - 1]]->add_actor(actor);
		}
		if (chunk_ids.count(chunk - 100000) > 0)
		{
			out[2] = chunk - 100000;
			chunks[chunk_ids[chunk - 100000]]->add_actor(actor);
		}
		if (chunk_ids.count(chunk - 100001) > 0)
		{
			out[3] = chunk - 100001;
			chunks[chunk_ids[chunk - 100001]]->add_actor(actor);
		}
	}
	
	return out;
}

void PhysicsChunk::compounded_update(std::unique_ptr<CriticalMutex>& mutex)
{
	
	std::vector<shared_ptr<PhysicsChunk>> chunks;
	int chunk_x = 0;
	int chunk_y = 0;

	while(CriticalMutex::all_frozen)
	{
		Sleep(1);
	}
	{
		
		//std::cout << "populating exiting actors: " <<  << std::endl;
		std::lock_guard<std::mutex> lock(chunk_map_mutex);
		for (auto i : mutex->chunk_ids)
		{
			chunk_x = chunk_map[i]->size.x;
			chunk_y = chunk_map[i]->size.y;
			chunks.push_back(chunk_map[i]);
		}
	}
	unordered_map<int, int> chunk_indicies = unordered_map<int, int>();
	unordered_map<int, shared_ptr<Actor>> actors;
	int c = 0;
	for(auto i : chunks)
	{
		chunk_indicies[i->id] = c;
		c++;
	}
	float delta_time = 1;
	for (auto i : chunks)
	{
		actors.insert(i->actors.begin(), i->actors.end());
	}

	auto lock_time = 0;
	auto limbo_list = 0;
	auto chunk_updates = 0;
	auto populating_exiting_actors = 0;


	while (true)
	{
		//for (auto i : chunks)
		//{
		//	for(auto j : mutex->chunk_interface[i->id]->limbo_list)
		//	{
		//		actors.insert({j->id, j});
		//	}
		//}
		
		auto start = std::chrono::high_resolution_clock::now();
		if (!CriticalMutex::all_frozen)
		{
			auto start2 = std::chrono::high_resolution_clock::now();
			std::unique_lock<std::mutex> lock(*mutex->mutex, std::defer_lock);
			if (lock.try_lock()) {
				auto end2 = std::chrono::high_resolution_clock::now();
				lock_time = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count();


				start2 = std::chrono::high_resolution_clock::now();
				if (!mutex->limbo_list.empty()) {
					for (auto& i : mutex->limbo_list)
					{
						for (auto j : i.second) {
							chunks[chunk_indicies[i.first]]->actors[j->getId()] = j;
							chunks[chunk_indicies[i.first]]->active = true;
							actors[j->getId()] = j;
						}
					}
					mutex->limbo_list.clear();
				}
				end2 = std::chrono::high_resolution_clock::now();
				limbo_list = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count();


				start2 = std::chrono::high_resolution_clock::now();

				

				int count = 0;
				for (auto& i : chunks)
				{
					//count++;
					//auto start2 = std::chrono::high_resolution_clock::now();
					//auto start2 = std::chrono::high_resolution_clock::now();
					if(!i->actors.empty())
						i->update(delta_time, *mutex->chunk_interface[i->id]);
						
					//auto end2 = std::chrono::high_resolution_clock::now();
					//std::cout << "Chunk " << count << ":" << std::chrono::duration_cast<std::chrono::nanoseconds>(end2 - start2).count() << std::endl;
				}

				for (auto& a : actors)
				{
					if(a.second != nullptr)
						a.second->update();
				}

				//for (auto i = --chunks.end(); i != chunks.begin(); --i)
				//{
				//	(*i)->update(delta_time, *mutex->chunk_interface[(*i)->id]);
				//}
				
				end2 = std::chrono::high_resolution_clock::now();
				chunk_updates = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count();
				

				start2 = std::chrono::high_resolution_clock::now();

				std::unordered_map<int, unordered_map<int, shared_ptr<Actor>>> chunk_add_map;
				for (auto& i : mutex->chunk_interface)
				{
					if (!i.second->exiting_actors.empty()) {

						bool bulk = i.second->exiting_actors.size() > 100;
						for (auto& j : i.second->exiting_actors)
						{
							if (actors[j] != nullptr) {
								
								auto arr = place_actor(actors[j], chunk_indicies, chunk_x, chunk_y, chunks, bulk);

								if (arr[0] == -1 || arr[1] == -1 || arr[2] == -1 || arr[3] == -1)
								{
									mutex->exiting_actors.insert(j);
									if (arr[0] == arr[1] && arr[1] == arr[2] && arr[2] == arr[3])
									{
										actors.erase(j);
										continue;
									}
								}
								if (bulk) {
									for (auto k : arr)
									{
										if (k != -1)
											chunk_add_map[chunk_indicies[k]].insert({ j, actors[j] });
									}
								}
							}
						}
						if (bulk) {
							for (auto& i : chunk_add_map)
							{

								chunks[i.first]->actors.insert(i.second.begin(), i.second.end());
								//TODO: FIX ^^^^^
							}
						}
						i.second->exiting_actors.clear();
					}
					if(!i.second->for_deletion.empty())
					{
						for (auto b : i.second->for_deletion)
						{
							if (actors[b] != nullptr) {
								actors.erase(b);
								mutex->for_deletion.insert(b);
							}
						}
					}
				}

				end2 = std::chrono::high_resolution_clock::now();
				populating_exiting_actors = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count();


			}
		}	
		Sleep(.1f);
		auto end = std::chrono::high_resolution_clock::now();
		auto runtime = (float)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		delta_time = runtime / 6944.4;
		if (delta_time > 1.f)
			delta_time = 1.f;
		//if (delta_time > 10)
			//std::cout << "Lock Time: " << lock_time << "\n" << "Limbo List: " << limbo_list << "\n" << "Chunk Updates: " << chunk_updates << "\n" << "Populating Exiting Actors: " << populating_exiting_actors << "\n" << "Total: " << runtime << "\n" << "Delta Time: " << delta_time << "\n" << std::endl;

	}
}

void PhysicsChunk::update(float deltaTime, ChunkInterface& ct)
{

	if (!actors.empty()) {
		if (!CriticalMutex::all_frozen) {
			auto temp = physics_update(deltaTime);
			auto exit = temp.first;
			auto for_deletion = temp.second;
			while (!exit.empty())
			{
				auto id = exit.front();
				actors.erase(id);
				exiting_actors.push_back(id);
				exit.pop();
			}

			while (!for_deletion.empty())
			{
				actors.erase(for_deletion.front());
				ct.for_deletion.insert(for_deletion.front());
				for_deletion.pop();
			}
			
			for (auto b : exiting_actors)
			{
				ct.exiting_actors.insert(b);
			}
			exiting_actors.clear();
		}
	} else
	{
		active = false;
	}
}

void PhysicsChunk::perpetual_update(float delta_time, ChunkInterface& ct, std::mutex& m)
{
	while (true) {
		Sleep(1);
		update(delta_time, ct);
	}
}


std::pair<std::queue<int>, std::queue<int>> PhysicsChunk::physics_update(float delta_time)
{
	if (actors.empty()) {
		active = false;
		return {};
	}
	
	//std::deque<shared_ptr<Actor>> temp_actors;
	//
	//for (auto b : actors) {
	//	temp_actors.push_back(b.second);
	//}
	bool act = false;
	collision_stack = queue<Collision>();
	for (const auto& b1 : actors) {
		//std::cout << "[" << pos.x/(size.x/2) << ", " << pos.y/(size.y/2) << "]" << b1.first << std::endl;
		//temp_actors.pop_front();
		if (b1.second->ball.active || b1.second->ball.always_active) {
			act = true;
			for (const auto& b2 : actors) {
				auto coll = Ball::collide(b1.second->ball, b2.second->ball);
				if (coll.collision)
					collision_stack.push(coll);
			}
		}
	}
	active = act;
	/*resolve collisions*/ {
		const auto temp = collision_stack;
		while (!collision_stack.empty()) {


			auto collision = collision_stack.front();
			collision_stack.pop();
			auto& b1 = actors[collision.ball1]->ball;
			auto& b2 = actors[collision.ball2]->ball;

			std::lock(*actors[collision.ball1]->mutex, *actors[collision.ball2]->mutex);
			std::lock_guard<std::mutex> lck1(*actors[collision.ball1]->mutex, std::adopt_lock);
			std::lock_guard<std::mutex> lck2(*actors[collision.ball2]->mutex, std::adopt_lock);
			//if (std::try_lock(b1_lock, b2_lock)) {
				//auto start = std::chrono::high_resolution_clock::now();
				///std::cout << "locked b1 & b2" << std::endl;
				//Complex Maths
			const auto normal = -(b2.pos - b1.pos) / sqrt(Ball::magsq((b2.pos - b1.pos)));
			const auto tangent = sf::Vector2f(-normal.y, normal.x);


			const auto tan_dp1 = (b1.vel.x) * tangent.x + (b1.vel.y) * tangent.y;
			const auto tan_dp2 = (b2.vel.x) * tangent.x + (b2.vel.y) * tangent.y;

			const auto norm_dp1 = (b1.vel.x) * normal.x + (b1.vel.y) * normal.y;
			const auto norm_dp2 = (b2.vel.x) * normal.x + (b2.vel.y) * normal.y;

			////Conservation of momentum
			const auto m1 = (norm_dp1 * (b1.mass - b2.mass) + 2.f * b2.mass * norm_dp2) / (b1.mass + b2.mass);
			const auto m2 = (norm_dp2 * (b2.mass - b1.mass) + 2.f * b1.mass * norm_dp1) / (b1.mass + b2.mass);

			//std::cout << 1 << ": " << std::this_thread::get_id() << std::endl;
			b1.collisionStack.insert({
				b2.id,
				{
					(delta_time < 10 ? delta_time : 10),
					collision.ball1_overlap,
					(((tangent * tan_dp1) + (normal * m1)) * (b1.elasticity * b2.elasticity))
				}
			});
			//std::cout << 2 << ": " << std::this_thread::get_id() << std::endl;
			b2.collisionStack.insert({
				b1.id,
				{
					(delta_time < 10 ? delta_time : 10),
					collision.ball2_overlap,
					(((tangent * tan_dp2) + (normal * m2)) * (b1.elasticity * b2.elasticity))
				}
			});
			//auto end = std::chrono::high_resolution_clock::now();
			//std::cout << "Double lock time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
		//} else
		//{
		//	//std::cout << "Actor Lock Blocked" << std::endl;
		//}
		}
		collision_stack = temp;


		if (is_border)
			for (auto b1 : actors) {
				if (b1.second->ball.active || b1.second->ball.always_active) {
					if (b1.second->ball.pos.x + b1.second->ball.rad * 2 > (pos.x + size.x) && bounds.right) {
						b1.second->ball.vel.x *= -1 * b1.second->ball.elasticity;
						b1.second->ball.pos.x = (pos.x + size.x) - b1.second->ball.rad * 2;
					}
					if (b1.second->ball.pos.x - b1.second->ball.rad * 2 < (pos.x) && bounds.left) {
						b1.second->ball.vel.x *= -1 * b1.second->ball.elasticity;
						b1.second->ball.pos.x = (pos.x) + b1.second->ball.rad * 2;
					}
					if (b1.second->ball.pos.y + b1.second->ball.rad * 2 > (pos.y + size.y) && bounds.bottom) {
						b1.second->ball.vel.y *= -1 * b1.second->ball.elasticity;
						b1.second->ball.pos.y = (pos.y + size.y) - b1.second->ball.rad * 2;
					}
					if (b1.second->ball.pos.y - b1.second->ball.rad * 2 < (pos.y) && bounds.top) {
						b1.second->ball.vel.y *= -1 * b1.second->ball.elasticity;
						b1.second->ball.pos.y = (pos.y) + b1.second->ball.rad * 2;
					}
				}
			}
		//std::cout << bounds.left << ", " << bounds.right << ", " << bounds.top << ", " << bounds.bottom << std::endl;

		std::queue<int> temp_exit;
		std::queue<int> for_deletion;
		ray_update_buffer += delta_time;
		if (ray_update_buffer > 10.f)
			ray_update_buffer = 0;
		for (auto b : actors) {
			if (b.second->ball.active || b.second->ball.always_active) {
				std::unique_lock <std::mutex > b_lock(*b.second->mutex);
				if (ray_update_buffer == 0.f) 
					b.second->ray_cast(actors);
				b.second->ball.update(delta_time);
				if (((b.second->ball.pos.x < pos.x) && !bounds.left) || ((b.second->ball.pos.x > pos.x + size.x) && !bounds.right) || ((b.second->ball.pos.y < pos.y) && !bounds.top) || ((b.second->ball.pos.y > pos.y + size.y) && !bounds.bottom))
				{
					temp_exit.push(b.first);
					//remove_actor(b.first);
				}
				if(b.second->for_deletion)
				{
					for_deletion.push(b.first);
				}
			}
		}
		return { temp_exit, for_deletion };
	}
	return {};	
}



void PhysicsChunk::draw( sf::RenderWindow& window)
{
	
	for (auto b : actors) {
		b.second->ball.draw(window);
	}
	
	/*for (int i = 0; i < collision_stack.size(); i++) {
		Collision temp = collision_stack.front();
		collision_stack.pop();
		drawLine(actors[temp.actor1]->pos, actors[temp.actor2]->pos, sf::Color::Red, window);
	}*/
}

bool PhysicsChunk::isInside(const sf::Vector2f obj_pos)
{
	return (obj_pos.x > pos.x && obj_pos.x < pos.x + size.x && obj_pos.y > pos.y && obj_pos.y < pos.y + size.y);
}

//void PhysicsChunk::add_actor(int id, const Actor& actor)
//{
//	actors.insert({id, std::make_shared<Actor>(actor) });
//}

void PhysicsChunk::add_actor(int id, const shared_ptr<Actor>& actor)
{
	if(actor != nullptr)
		actors.insert({ id, actor });
}

void PhysicsChunk::add_actor(const shared_ptr<Actor>& actor)
{
	if (actor != nullptr)
		actors.insert({ actor->getId(), actor});
}

void PhysicsChunk::add_actor(const std::list<shared_ptr<Actor>>& actor)
{
	for(auto b : actor)
	{
		if (b != nullptr)
			add_actor(b);
	}
}