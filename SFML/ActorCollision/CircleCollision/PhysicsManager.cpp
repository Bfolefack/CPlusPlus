#include "CriticalMutex.h"
#include "PhysicsManager.h"
#include "ChunkInterface.h"
#include "Actor.h"
#include "PhysicsChunk.h"

#include <array>
#include <iostream>
#include <unordered_set>
#include "Border.h"
#include <thread>
#include <Windows.h>

using std::shared_ptr;
using std::unordered_map;



PhysicsManager::PhysicsManager(int w, int h, int cw, int ch)
{
	width = w;
	height = h;
	chunk_width = cw;
	chunk_height = ch;



	actors = std::unordered_map<int, shared_ptr<Actor>>();
	std::queue<int> chunks;
	for (int i = 0; i < width / cw; ++i)
	{
		for (int j = 0; j < height / ch; ++j)
		{
			PhysicsChunk pc = PhysicsChunk(i + j * 100000, sf::Vector2f(i * cw, j * ch), sf::Vector2f(cw * 2, ch * 2), Border(i == 0, i == (width / cw) - 2, j == 0, j == (height / ch - 2)));
			PhysicsChunk::chunk_map.insert({ i + j * 100000, std::make_shared<PhysicsChunk>(pc)});
			chunks.push( i + j * 100000);
		}
	}


	selectedActor = nullptr;
	velSelectedActor = nullptr;
	mousePos = sf::Vector2f(0, 0);
	circles = sf::VertexArray(sf::Quads, 0);
	circleTexture.loadFromFile("circle.png");
	circleTexture.setSmooth(true);
	chunks_to_threads = std::unordered_map<int, int>();
	chunk_threads = std::unordered_map<int, std::unique_ptr<CriticalMutex>>();

	//std::mutex m;
	int chunks_per_thread = (PhysicsChunk::chunk_map.size()/MAX_THREADS + 1);
	CriticalMutex::all_frozen = true;

	int count = 0;
	while (!chunks.empty())
	{
		CriticalMutex critical_mutex;
		for (int i = 0; i < chunks_per_thread && !chunks.empty(); ++i)
		{
			ChunkInterface ci;
			critical_mutex.chunk_interface.insert({ chunks.front(), std::make_unique<ChunkInterface>(ci)});
			chunks_to_threads.insert({ chunks.front(), count });
			critical_mutex.chunk_ids.push_back(chunks.front());
			chunks.pop();
		}

		chunk_threads.insert({ count, std::make_unique<CriticalMutex>(std::move(critical_mutex)) });
		chunk_threads[count]->thread = std::thread(&PhysicsChunk::compounded_update, std::ref(chunk_threads[count]));
		count++;
		std::cout << "Thread " << count << " started" << std::endl;
	}

	/*
	for (auto i : map)
	{
		//std::cout << i.first << std::endl;
		CriticalMutex c;

		//ct.exiting_actors = std::list<int>();
		//ct.limbo_list = std::list < std::shared_ptr<Actor>>();
		//auto ct_ptr = std::make_shared<ChunkInterface>(std::move(ct));
		//auto ct = ChunkInterface(*i.second, i.first);
		//ct_ptr->thread = std::make_shared<std::thread>(std::thread(&PhysicsChunk::perpetual_update, i.second, 0.0f, ct_ptr));

		//std::lock_guard<std::mutex> lock(*c.mutex);
		c.chunk_interface = std::make_unique<ChunkInterface>(ChunkInterface(*i.second, i.first));
		c.chunk_interface->thread = std::thread(&PhysicsChunk::compounded_update,i.second, 1, std::ref(*c.chunk_interface), std::ref(*c.mutex));

		chunk_interface.insert({ i.first, std::make_unique<CriticalMutex>(std::move(c)) });

		//chunk_interface.insert({i.first, )});
		//chunk_interface[i.first] = ChunkInterface();
	}
	*/


	CriticalMutex::all_frozen = false;
	Sleep(100);
	//for (auto b : actors)
	//{
	//	for(auto i : place_actor(b.first))
	//	{
	//		chunk_interface[i]->limbo_list.push_back(b.second);
	//	}
	//}

	//vector<shared_ptr<std::thread>> threadQueue = std::vector<shared_ptr<std::thread>>();
}

bool PhysicsManager::is_inside_box(sf::Vector2f pos, sf::Vector2f box_pos, sf::Vector2f box_size)
{
	return pos.x > box_pos.x && pos.x < box_pos.x + box_size.x && pos.y > box_pos.y && pos.y < box_pos.y + box_size.y;
}

void PhysicsManager::draw(sf::RenderWindow& window) 
{

	//std::lock_guard<std::mutex> lock(mutex);
	circles = sf::VertexArray(sf::Quads, 400);
	const auto cTX = circleTexture.getSize().x;
	const auto cTY = circleTexture.getSize().y;
	//std::cout << circleTexture.getSize().x << ", " << circleTexture.getSize().y << std::endl;
	for (const auto b : actors)
	{
		if (b.second != nullptr && is_inside_box(b.second->ball.pos, window.getView().getCenter() - (window.getView().getSize()/2.f), window.getView().getSize())){
			sf::Vertex tl;
			tl.position = b.second->ball.pos + sf::Vector2f(-b.second->ball.rad, -b.second->ball.rad);
			
			tl.texCoords = sf::Vector2f(0, 0);
			sf::Vertex tr;
			tr.position = b.second->ball.pos + sf::Vector2f(b.second->ball.rad, -b.second->ball.rad);
			tr.texCoords = sf::Vector2f(cTX, 0);
			sf::Vertex br;
			br.position = b.second->ball.pos + sf::Vector2f(b.second->ball.rad, b.second->ball.rad);
			br.texCoords = sf::Vector2f(cTX, cTY);
			sf::Vertex bl;
			bl.position = b.second->ball.pos + sf::Vector2f(-b.second->ball.rad, b.second->ball.rad);
			bl.texCoords = sf::Vector2f(0, cTY);
			if(b.second->ball.active)
			{
				tl.color = sf::Color::Red;
				tr.color = sf::Color::Red;
				br.color = sf::Color::Red;
				bl.color = sf::Color::Red;
			} else
			{
				tl.color = sf::Color::White;
				tr.color = sf::Color::White;
				br.color = sf::Color::White;
				bl.color = sf::Color::White;
			}
			circles.append(tl);
			circles.append(tr);
			circles.append(br);
			circles.append(bl);
		}
	}
	//std::cout << circles.getVertexCount() << std::endl;
	window.draw(circles,  &circleTexture);
	circles.clear();
	//for (auto b : actors)
	//{
	//	b.second->draw(window);
	//}
	mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	if (velSelectedActor != nullptr) {
		drawLine(velSelectedActor->ball.pos, mousePos, sf::Color::Blue, window);
	}
}

void PhysicsManager::drawLine(sf::Vector2f point1, sf::Vector2f point2, sf::Color color, sf::RenderWindow& window) {
	sf::Vertex line[2];
	line[0].position = point1;
	line[1].position = point2;
	line[0].color = color;
	line[1].color = color;
	window.draw(line, 2, sf::Lines);
}

void PhysicsManager::thread_update()
{
	
}

void PhysicsManager::perpetual_update()
{
	while (true)
	{
		//std::lock_guard<std::mutex> lock(mutex);
		update();
	}
}

 

void PhysicsManager::update()
{
	//auto start = std::chrono::high_resolution_clock::now();
	if (selectedActor != nullptr) {
		std::lock_guard<std::mutex> lock(*selectedActor->mutex);
		selectedActor->ball.pos = mousePos;
	}
	//auto end = std::chrono::high_resolution_clock::now();
	//std::cout << "selected_actor_check: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	

	auto start = std::chrono::high_resolution_clock::now();
	auto exiting_actors = std::unordered_set<int>();
	CriticalMutex::all_frozen = true;
	for(auto i = chunk_threads.begin(); i != chunk_threads.end(); ++i)
	{
		std::lock_guard<std::mutex> lock(*i->second->mutex);
		if (!i->second->exiting_actors.empty()) {
			//std::cout << i->first << std::endl;
			exiting_actors.insert(i->second->exiting_actors.begin(), i->second->exiting_actors.end());
			i->second->exiting_actors.clear();
		}
	}
	CriticalMutex::all_frozen = false;
	auto end = std::chrono::high_resolution_clock::now();
	//std::cout << "populating exiting actors: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	
	start = std::chrono::high_resolution_clock::now();
	re_add_actor(exiting_actors);
	end = std::chrono::high_resolution_clock::now();
	//std::cout << "re add actor: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "\n\n" << std::endl;
	Sleep(1);
}

std::array<int, 4> PhysicsManager::place_actor(const int b)
{
	const auto actor = actors[b];
	const auto chunk_x = static_cast<int>(actor->ball.pos.x) / chunk_width;
	const auto chunk_y = static_cast<int>(actor->ball.pos.y) / chunk_height;
	const int chunk = chunk_x + (chunk_y * 100000);
	//chunk {0 0, 0 -1, -1 0, -1 -1}
	auto out = std::array<int, 4>({-1, -1, -1, -1});
	if(chunks_to_threads.count(chunk) <= 0)
	{
		actors.erase(b);
		return out;
	}
	//map[chunk]->add_actor(b, actor);
	out[0] = chunk;
	if(chunk_x > 0 && chunk_y > 0)
	{
		out[1] = chunk - 1;
		//map[chunk - 1]->add_actor(b, actor);
		out[2] = chunk - 100000;
		//map[chunk - 100000]->add_actor(b, actor);
		out[3] = chunk - 100001;
		//map[chunk - 100001]->add_actor(b, actor);
	} else if(chunk_x > 0)
	{
		out[1] = chunk - 1;
		//map[chunk - 1]->add_actor(b, actor);
	} else if(chunk_y > 0)
	{
		out[2] = chunk - 100000;
		//map[chunk - 100000]->add_actor(b, actor);
	}
	return out;
}

void PhysicsManager::add_actor(std::vector<shared_ptr<Actor>> added_actors)
{
	CriticalMutex::all_frozen = true;
	auto chunk_add_map = std::unordered_map<int, std::queue<int>>();
	//for (auto i = chunk_threads.begin(); i != chunk_threads.end() ; ++i)
	//{
	//	const int n = i->first;
	//	//chunk_add_map.insert(n, );
	//}
	for (const shared_ptr<Actor> actor : added_actors)
	{
		actor->setId(static_cast<int>(actors.size()));
		actors[static_cast<int>(actors.size())] = actor;

		auto list = place_actor(actor->getId());
		for (auto i : list)
		{
			if(i != -1)
			{
				if (chunk_add_map.count(i) <= 0)
					chunk_add_map[i] = std::queue<int>();
				chunk_add_map[i].push(actor->getId());
			}
		}
	}
	std::unordered_map<int, std::list<int>> thread_add_map;
	for (auto i = chunk_add_map.begin(); i != chunk_add_map.end(); ++i)
	{
		thread_add_map[chunks_to_threads[i->first]].push_back(i->first);
	}

	for (const auto i : thread_add_map)
	{
		std::unique_lock<std::mutex> lock(*(chunk_threads[i.first]->mutex));
		for (const int j : i.second)
		{
			const int k = chunk_add_map[j].front();
			chunk_threads[i.first]->limbo_list.insert({ j, std::list<shared_ptr<Actor>>() });
			while (!chunk_add_map[j].empty())
			{
				chunk_threads[i.first]->limbo_list[j].push_back(actors[chunk_add_map[j].front()]);
				chunk_add_map[j].pop();
			}			
		}
	}
	CriticalMutex::all_frozen = false;
}

void PhysicsManager::re_add_actor(std::unordered_set<int> added_actors)
{
	auto chunk_add_map = std::unordered_map<int, std::queue<int>>();
	//for (auto i = chunk_interface.begin(); i != chunk_interface.end(); ++i)
	//{
	//	const int n = i->first;
	//	//chunk_add_map.insert(n, );
	//}
	auto start = std::chrono::high_resolution_clock::now();
	for (const int actor : added_actors)
	{
		if (actors[actor] != nullptr) {
			auto list = place_actor(actors[actor]->getId());
			for (auto i : list)
			{
				if (i != -1)
				{
					if (chunk_add_map.count(i) <= 0)
						chunk_add_map[i] = std::queue<int>();
					chunk_add_map[i].push(actors[actor]->getId());
				}
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	//std::cout << "populating chunk_add_map: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;


	start = std::chrono::high_resolution_clock::now();
	std::unordered_map<int, std::unordered_map<int, std::list<std::shared_ptr<Actor>>>> thread_add_map;
	for (auto i = chunk_add_map.begin(); i != chunk_add_map.end(); ++i)
	{
		int thread = chunks_to_threads[i->first];
		if(thread_add_map.count(thread) <= 0)
		{
			thread_add_map[thread] = std::unordered_map<int, std::list<std::shared_ptr<Actor>>>();
		}
		if(thread_add_map.count(i->first) == 0)
		{
			thread_add_map[thread][i->first] = std::list<std::shared_ptr<Actor>>();
		}
		while (!i->second.empty()) {
			thread_add_map[thread][i->first].push_back(actors[i->second.front()]);
			i->second.pop();
		}
		//thread_add_map[chunks_to_threads[i->first]]
	}
	end = std::chrono::high_resolution_clock::now();
	//std::cout << "populating thread_add_map: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	
	start = std::chrono::high_resolution_clock::now();
	for (const auto i : thread_add_map)
	{
		std::lock_guard<std::mutex> lock(*chunk_threads[i.first]->mutex);
		for (auto j : i.second) {
			if (chunk_threads[i.first]->limbo_list.count(j.first) <= 0)
			{
				chunk_threads[i.first]->limbo_list[j.first] = std::list<shared_ptr<Actor>>();
			}
			chunk_threads[i.first]->limbo_list[j.first].splice(chunk_threads[i.first]->limbo_list[j.first].end(), j.second);
		}
	}
	end = std::chrono::high_resolution_clock::now();
	//std::cout << "populating limbo_list: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

}

//void PhysicsManager::add_actor(
//  actor)
//{
//	actor.setId(actors.size());
//	auto id = actor.getId();
//	actors.insert({ id, std::make_shared<Actor>(actor) });
//	auto arr = place_actor(id);
//	for (auto i : arr)
//	{
//		if (i != -1)
//		{
//			auto thread = chunks_to_threads[i];
//			std::lock_guard<std::mutex> lock(*chunk_threads[thread]->mutex);
//			if (chunk_threads[thread]->limbo_list.count(i) <= 0)
//			{
//				chunk_threads[thread]->limbo_list[i] = std::list<shared_ptr<Actor>>();
//			}
//			chunk_threads[thread]->limbo_list[i].push_back(actors[id]);
//		}
//	}
//}

//void PhysicsManager::add_actor(Actor actor)
//{
//	add_actor(std::make_shared<Actor>(actor));
//}
//
//void PhysicsManager::re_add_actor(int id)
//{
//	auto list = place_actor(id);
//	auto arr = std::array<std::thread, 4>();
//
//
//	int count = 0;
//	for (int i = 0; i < list.size(); i++) {
//		if (list[i] != -1) {
//			arr[i] = std::thread(&PhysicsManager::threaded_add_actor, std::ref(*chunk_threads[list[i]]->mutex), std::ref(chunk_threads[list[i]]->chunk_interface->limbo_list), actors[id]);
//		}
//		else
//		{
//			count++;
//		}
//	}
//	while (count < list.size()) {
//		for (int i = 0; i < list.size(); i++) {
//			if (list[i] != -1) {
//				if (arr[i].joinable())
//				{
//					count++;
//					arr[i].join();
//					list[i] = -1;
//				}
//			}
//		}
//	}
//}

//void PhysicsManager::add_actor(shared_ptr<Actor> actor)
//{
//	//std::cout << actors.size() << "\n";
//	actor->getId() = static_cast<int>(actors.size());
//	actors[static_cast<int>(actors.size())] = std::make_shared<Actor>(* actor);
//
//	//auto start = std::chrono::high_resolution_clock::now();
//
//	auto list = place_actor(actor->getId());
//	auto arr = std::array<std::thread, 4>();
//
//
//	int count = 0;
//	for (int i = 0; i < list.size(); i++) {
//		if (list[i] != -1) {
//			arr[i] = std::thread(&PhysicsManager::threaded_add_actor, std::ref(*chunk_threads[list[i]]->mutex), std::ref(chunk_threads[list[i]]->chunk_interface->limbo_list), actor);
//		} else
//		{
//			count++;
//		}
//	}
//	while (count < list.size()) {
//		for (int i = 0; i < list.size(); i++) {
//			if (list[i] != -1) {
//				if (arr[i].joinable())
//				{
//					count++;
//					arr[i].join();
//					list[i] = -1;
//				}
//			}
//		}
//	}
//
//	//auto end = std::chrono::high_resolution_clock::now();
//	//std::cout << "Add Actor: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
//}

//void PhysicsManager::threaded_add_actor(std::mutex& mutex, std::list<shared_ptr<Actor>>& limbo_list, shared_ptr<Actor> actor)
//{
//	std::unique_lock<std::mutex> lock(mutex);
//	limbo_list.push_back(actor);
//	//return;
//}

int PhysicsManager::get_chunk(sf::Vector2f vec) const
{
	const auto chunk_x = static_cast<int>(vec.x) / chunk_width;
	const auto chunk_y = static_cast<int>(vec.y) / chunk_height;
	return chunk_x + (chunk_y * 100000);
}

void PhysicsManager::select_actor()
{
	for (auto b : actors) {
		if(b.second != nullptr)
			if (b.second->ball.isInside(mousePos)) {
				selectedActor = b.second;
				return;
			}
	}
}

void PhysicsManager::deselect_actor()
{
	if(selectedActor != nullptr)
		selectedActor->ball.active = true;
	selectedActor = nullptr;
}

void PhysicsManager::vel_select_actor()
{
	for (auto b : actors) {
		if(b.second != nullptr)
			if (b.second->ball.isInside(mousePos)) {
				velSelectedActor = b.second;
				return;
			}
	}
}

void PhysicsManager::vel_deselect_actor()
{
	if (velSelectedActor != nullptr) {

		std::lock_guard<std::mutex> lock(*velSelectedActor->mutex);
		velSelectedActor->ball.vel = ((velSelectedActor->ball.pos - mousePos));
		velSelectedActor->ball.active = true;
		auto arr = place_actor(velSelectedActor->getId());
		for (auto i : arr)
		{
			if (i != -1)
			{
				auto thread = chunks_to_threads[i];
				//std::lock_guard<std::mutex> lock(*chunk_threads[thread]->mutex);
				if (chunk_threads[thread]->limbo_list.count(i) <= 0)
				{
					chunk_threads[thread]->limbo_list[i] = std::list<shared_ptr<Actor>>();
				}
				chunk_threads[thread]->limbo_list[i].push_back(velSelectedActor);
			}
		}
		velSelectedActor = nullptr;
	}
}


