#include "CriticalMutex.h"
#include "PhysicsManager.h"
#include "ChunkInterface.h"
#include "Ball.h"
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



	balls = std::unordered_map<int, shared_ptr<Ball>>();
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


	selectedBall = nullptr;
	velSelectedBall = nullptr;
	mousePos = sf::Vector2f(0, 0);
	circles = sf::VertexArray(sf::Quads, 0);
	circleTexture.loadFromFile("circle.png");
	circleTexture.setSmooth(true);
	chunks_to_threads = std::unordered_map<int, int>();
	chunk_threads = std::unordered_map<int, std::unique_ptr<CriticalMutex>>();

	//std::mutex m;
	int chunks_per_thread = PhysicsChunk::chunk_map.size()/MAX_THREADS;
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

		//ct.exiting_balls = std::list<int>();
		//ct.limbo_list = std::list < std::shared_ptr<Ball>>();
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
	//for (auto b : balls)
	//{
	//	for(auto i : place_ball(b.first))
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
	for (const auto b : balls)
	{
		if (b.second != nullptr && is_inside_box(b.second->pos, window.getView().getCenter() - (window.getView().getSize()/2.f), window.getView().getSize())){
			sf::Vertex tl;
			tl.position = b.second->pos + sf::Vector2f(-b.second->rad, -b.second->rad);
			tl.color = sf::Color::White;
			tl.texCoords = sf::Vector2f(0, 0);
			sf::Vertex tr;
			tr.position = b.second->pos + sf::Vector2f(b.second->rad, -b.second->rad);
			tr.texCoords = sf::Vector2f(cTX, 0);
			tr.color = sf::Color::White;
			sf::Vertex br;
			br.position = b.second->pos + sf::Vector2f(b.second->rad, b.second->rad);
			br.texCoords = sf::Vector2f(cTX, cTY);
			br.color = sf::Color::White;
			sf::Vertex bl;
			bl.position = b.second->pos + sf::Vector2f(-b.second->rad, b.second->rad);
			bl.texCoords = sf::Vector2f(0, cTY);
			bl.color = sf::Color::White;
			circles.append(tl);
			circles.append(tr);
			circles.append(br);
			circles.append(bl);
		}
	}
	//std::cout << circles.getVertexCount() << std::endl;
	window.draw(circles,  &circleTexture);
	circles.clear();
	//for (auto b : balls)
	//{
	//	b.second->draw(window);
	//}
	mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	if (velSelectedBall != nullptr) {
		drawLine(velSelectedBall->pos, mousePos, sf::Color::Blue, window);
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
	if (selectedBall != nullptr) {
		std::lock_guard<std::mutex> lock(*selectedBall->mutex);
		selectedBall->pos = mousePos;
	}
	//auto end = std::chrono::high_resolution_clock::now();
	//std::cout << "selected_ball_check: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	

	auto start = std::chrono::high_resolution_clock::now();
	auto exiting_balls = std::unordered_set<int>();
	for(auto i = chunk_threads.begin(); i != chunk_threads.end(); ++i)
	{
		//std::cout << i->first << std::endl;
		exiting_balls.insert(i->second->exiting_balls.begin(), i->second->exiting_balls.end());
	}
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "populating exiting balls: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	
	start = std::chrono::high_resolution_clock::now();
	re_add_ball(exiting_balls);
	//CriticalMutex::afalse;
	end = std::chrono::high_resolution_clock::now();
	std::cout << "re add ball: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "\n\n" << std::endl;
}

std::array<int, 4> PhysicsManager::place_ball(const int b)
{
	const auto ball = balls[b];
	const auto chunk_x = static_cast<int>(ball->pos.x) / chunk_width;
	const auto chunk_y = static_cast<int>(ball->pos.y) / chunk_height;
	const int chunk = chunk_x + (chunk_y * 100000);
	//chunk {0 0, 0 -1, -1 0, -1 -1}
	auto out = std::array<int, 4>({-1, -1, -1, -1});
	if(chunks_to_threads.count(chunk) <= 0)
	{
		balls.erase(b);
		return out;
	}
	//map[chunk]->add_ball(b, ball);
	out[0] = chunk;
	if(chunk_x > 0 && chunk_y > 0)
	{
		out[1] = chunk - 1;
		//map[chunk - 1]->add_ball(b, ball);
		out[2] = chunk - 100000;
		//map[chunk - 100000]->add_ball(b, ball);
		out[3] = chunk - 100001;
		//map[chunk - 100001]->add_ball(b, ball);
	} else if(chunk_x > 0)
	{
		out[1] = chunk - 1;
		//map[chunk - 1]->add_ball(b, ball);
	} else if(chunk_y > 0)
	{
		out[2] = chunk - 100000;
		//map[chunk - 100000]->add_ball(b, ball);
	}
	return out;
}

void PhysicsManager::add_ball(std::vector<shared_ptr<Ball>> added_balls)
{
	CriticalMutex::all_frozen = true;
	auto chunk_add_map = std::unordered_map<int, std::queue<int>>();
	//for (auto i = chunk_threads.begin(); i != chunk_threads.end() ; ++i)
	//{
	//	const int n = i->first;
	//	//chunk_add_map.insert(n, );
	//}
	for (const shared_ptr<Ball> ball : added_balls)
	{
		ball->id = static_cast<int>(balls.size());
		balls[static_cast<int>(balls.size())] = ball;

		auto list = place_ball(ball->id);
		for (auto i : list)
		{
			if(i != -1)
			{
				if (chunk_add_map.count(i) <= 0)
					chunk_add_map[i] = std::queue<int>();
				chunk_add_map[i].push(ball->id);
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
			while (!chunk_add_map[j].empty())
			{
				const int k = chunk_add_map[j].front();
				chunk_threads[i.first]->chunk_interface[j]->limbo_list.push_back(balls[k]);
				chunk_add_map[j].pop();
			}
		}
		while (!(chunk_add_map[i.first]).empty())
		{
			chunk_threads[chunks_to_threads[i.first]]->chunk_interface[i.first]->limbo_list.push_back(balls[chunk_add_map[i.first].front()]);
			chunk_add_map[i.first].pop();
		}
	}
	CriticalMutex::all_frozen = false;
}

void PhysicsManager::re_add_ball(std::unordered_set<int> added_balls)
{
	auto chunk_add_map = std::unordered_map<int, std::queue<int>>();
	//for (auto i = chunk_interface.begin(); i != chunk_interface.end(); ++i)
	//{
	//	const int n = i->first;
	//	//chunk_add_map.insert(n, );
	//}
	auto start = std::chrono::high_resolution_clock::now();
	for (const int ball : added_balls)
	{
		if (balls[ball] != nullptr) {
			auto list = place_ball(balls[ball]->id);
			for (auto i : list)
			{
				if (i != -1)
				{
					if (chunk_add_map.count(i) <= 0)
						chunk_add_map[i] = std::queue<int>();
					chunk_add_map[i].push(balls[ball]->id);
				}
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	//std::cout << "populating chunk_add_map: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;


	start = std::chrono::high_resolution_clock::now();
	std::unordered_map<int, std::unordered_map<int, std::list<std::shared_ptr<Ball>>>> thread_add_map;
	for (auto i = chunk_add_map.begin(); i != chunk_add_map.end(); ++i)
	{
		int thread = chunks_to_threads[i->first];
		if(thread_add_map.count(thread) <= 0)
		{
			thread_add_map[thread] = std::unordered_map<int, std::list<std::shared_ptr<Ball>>>();
		}
		if(thread_add_map.count(i->first) == 0)
		{
			thread_add_map[thread][i->first] = std::list<std::shared_ptr<Ball>>();
		}
		while (!i->second.empty()) {
			thread_add_map[thread][i->first].push_back(balls[i->second.front()]);
			i->second.pop();
		}
		//thread_add_map[chunks_to_threads[i->first]]
	}
	end = std::chrono::high_resolution_clock::now();
	//std::cout << "populating thread_add_map: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	
	start = std::chrono::high_resolution_clock::now();
	for (const auto i : thread_add_map)
	{
		for (auto j : i.second) {
			if (chunk_threads[i.first]->limbo_list.count(j.first) <= 0)
			{
				chunk_threads[i.first]->limbo_list[j.first] = std::list<shared_ptr<Ball>>();
			}
			chunk_threads[i.first]->limbo_list[j.first].splice(chunk_threads[i.first]->limbo_list[j.first].end(), j.second);
		}
	}
	end = std::chrono::high_resolution_clock::now();
	//std::cout << "populating limbo_list: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

}

//void PhysicsManager::add_ball(Ball ball)
//{
//	add_ball(std::make_shared<Ball>(ball));
//}
//
//void PhysicsManager::re_add_ball(int id)
//{
//	auto list = place_ball(id);
//	auto arr = std::array<std::thread, 4>();
//
//
//	int count = 0;
//	for (int i = 0; i < list.size(); i++) {
//		if (list[i] != -1) {
//			arr[i] = std::thread(&PhysicsManager::threaded_add_ball, std::ref(*chunk_threads[list[i]]->mutex), std::ref(chunk_threads[list[i]]->chunk_interface->limbo_list), balls[id]);
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

//void PhysicsManager::add_ball(shared_ptr<Ball> ball)
//{
//	//std::cout << balls.size() << "\n";
//	ball->id = static_cast<int>(balls.size());
//	balls[static_cast<int>(balls.size())] = std::make_shared<Ball>(* ball);
//
//	//auto start = std::chrono::high_resolution_clock::now();
//
//	auto list = place_ball(ball->id);
//	auto arr = std::array<std::thread, 4>();
//
//
//	int count = 0;
//	for (int i = 0; i < list.size(); i++) {
//		if (list[i] != -1) {
//			arr[i] = std::thread(&PhysicsManager::threaded_add_ball, std::ref(*chunk_threads[list[i]]->mutex), std::ref(chunk_threads[list[i]]->chunk_interface->limbo_list), ball);
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
//	//std::cout << "Add Ball: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
//}

//void PhysicsManager::threaded_add_ball(std::mutex& mutex, std::list<shared_ptr<Ball>>& limbo_list, shared_ptr<Ball> ball)
//{
//	std::unique_lock<std::mutex> lock(mutex);
//	limbo_list.push_back(ball);
//	//return;
//}

int PhysicsManager::get_chunk(sf::Vector2f vec) const
{
	const auto chunk_x = static_cast<int>(vec.x) / chunk_width;
	const auto chunk_y = static_cast<int>(vec.y) / chunk_height;
	return chunk_x + (chunk_y * 100000);
}

void PhysicsManager::select_ball()
{
	for (auto b : balls) {
		if(b.second != nullptr)
			if (b.second->isInside(mousePos)) {
				selectedBall = b.second;
				return;
			}
	}
}

void PhysicsManager::deselect_ball()
{
	selectedBall = nullptr;
}

void PhysicsManager::vel_select_ball()
{
	for (auto b : balls) {
		if(b.second != nullptr)
			if (b.second->isInside(mousePos)) {
				velSelectedBall = b.second;
				return;
			}
	}
}

void PhysicsManager::vel_deselect_ball()
{
	if (velSelectedBall != nullptr) {

		std::lock_guard<std::mutex> lock(*velSelectedBall->mutex);
		velSelectedBall->vel = ((velSelectedBall->pos - mousePos));
		velSelectedBall = nullptr;
	}
}


