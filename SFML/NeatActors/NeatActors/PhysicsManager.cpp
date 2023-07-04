#include "CriticalMutex.h"
#include "PhysicsManager.h"
#include "ChunkInterface.h"
#include "PhysicsCore.h"
#include "PhysicsChunk.h"

#include <array>
#include <iostream>
#include <unordered_set>
#include "Border.h"
#include <thread>
#include <Windows.h>

#include "Actor.h"
#include "PlantActor.h"

using std::shared_ptr;
using std::unordered_map;

int PhysicsManager::global_selected_actor = -1;


PhysicsManager::PhysicsManager(int w, int h, int cw, int ch)
{
	width = w;
	height = h;
	chunk_width = cw;
	chunk_height = ch;
	population = Population<Actor>();


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

	textures.push_back(sf::Texture());
	textures.push_back(sf::Texture());
	textures.push_back(sf::Texture());
	textures[0].loadFromFile("chevron.png");
	textures[1].loadFromFile("circle.png");
	textures[2].loadFromFile("square.png");
	if(font.loadFromFile("../../ActorCollision/CircleCollision/COMIC.TTF"))
	{
		std::cout << "Loaded Font: " << font.getInfo().family << std::endl;
	}

	for (int i = 0; i < 2; ++i)
	{
		texture_groups.push_back(std::unordered_set<int>());
		vertex_arrays.push_back(sf::VertexArray(sf::Quads));
	}
	chunks_to_threads = std::unordered_map<int, int>();
	chunk_threads = std::unordered_map<int, std::unique_ptr<CriticalMutex>>();

	//std::mutex m;
	int chunks_per_thread = (PhysicsChunk::chunk_map.size()/MAX_THREADS + 1);
	CriticalMutex::all_frozen = true;

	int count = 0;
	current_actor = 0;
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
		//ct.limbo_list = std::list < std::shared_ptr<PhysicsCore>>();
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

void PhysicsManager::display_sprites(sf::RenderWindow& window, float zoom, float min_size, std::vector<int> populations)
{
	for (int i = 0; i < texture_groups.size(); i++) {
		vertex_arrays[i] = sf::VertexArray(sf::Quads, texture_groups[i].size() * 4);
		const auto cTX = textures[i].getSize().x;
		const auto cTY = textures[i].getSize().y;
		populations.push_back(0);
		for (const auto j : texture_groups[i])
		{
			if (actors.count(j) > 0) {
				populations[i]++;
				const auto actor = actors.at(j);
				//if (PlantActor* c = dynamic_cast<PlantActor*>(actor.get()))
				//{
				//	total += c->size;
				//}
				if (actor != nullptr && is_inside_box(actor->ball.pos, window.getView().getCenter() - (window.getView().getSize() / 2.f), window.getView().getSize()) && actor->ball.rad / zoom > min_size) {
					sf::Vertex tl;
					tl.position = actor->ball.pos + sf::Vector2f(cosf(3.1415f * 3 / 4 + actor->facing), sinf(3.1415f * 3 / 4 + actor->facing)) * actor->ball.rad;
					tl.texCoords = sf::Vector2f(0, 0);
					tl.color = actor->sprite_color;
					sf::Vertex tr;
					tr.position = actor->ball.pos + sf::Vector2f(cosf(3.1415f / 4 + actor->facing), sinf(3.1415f / 4 + actor->facing)) * actor->ball.rad;
					tr.texCoords = sf::Vector2f(cTX, 0);
					tr.color = actor->sprite_color;
					sf::Vertex br;
					br.position = actor->ball.pos + sf::Vector2f(cosf(-3.1415f / 4 + actor->facing), sinf(-3.1415f / 4 + actor->facing)) * actor->ball.rad;
					br.texCoords = sf::Vector2f(cTX, cTY);
					br.color = actor->sprite_color;
					sf::Vertex bl;
					bl.position = actor->ball.pos + sf::Vector2f(cosf(-3.1415f * 3 / 4 + actor->facing), sinf(-3.1415f * 3 / 4 + actor->facing)) * actor->ball.rad;
					bl.texCoords = sf::Vector2f(0, cTY);
					bl.color = actor->sprite_color;

					vertex_arrays[i].append(tl);
					vertex_arrays[i].append(tr);
					vertex_arrays[i].append(br);
					vertex_arrays[i].append(bl);
				}
			}
		}
		window.draw(vertex_arrays[i], &textures[i]);
	}
}

void PhysicsManager::draw_zoom_indicators(sf::RenderWindow& window, float zoom, float min_size, sf::VertexArray zoom_indicators, const unsigned cTX, const unsigned cTY)
{
	for (auto i : actors)
	{
		if (i.second != nullptr && is_inside_box(i.second->ball.pos, window.getView().getCenter() - (window.getView().getSize() / 2.f), window.getView().getSize())) {
			if(i.second->ball.rad/zoom < min_size)
			{
				auto actor = i.second;
				sf::Vertex tl;
				tl.position = actor->ball.pos + sf::Vector2f(cosf(3.1415f * 3 / 4 + actor->facing), sinf(3.1415f * 3 / 4 + actor->facing)) * zoom * min_size;
				tl.texCoords = sf::Vector2f(0, 0);
				tl.color = actor->sprite_color;
				sf::Vertex tr;
				tr.position = actor->ball.pos + sf::Vector2f(cosf(3.1415f / 4 + actor->facing), sinf(3.1415f / 4 + actor->facing)) * zoom * min_size;
				tr.texCoords = sf::Vector2f(cTX, 0);
				tr.color = actor->sprite_color;
				sf::Vertex br;
				br.position = actor->ball.pos + sf::Vector2f(cosf(-3.1415f / 4 + actor->facing), sinf(-3.1415f / 4 + actor->facing)) * zoom * min_size;
				br.texCoords = sf::Vector2f(cTX, cTY);
				br.color = actor->sprite_color;
				sf::Vertex bl;
				bl.position = actor->ball.pos + sf::Vector2f(cosf(-3.1415f * 3 / 4 + actor->facing), sinf(-3.1415f * 3 / 4 + actor->facing)) * zoom * min_size;
				bl.texCoords = sf::Vector2f(0, cTY);
				bl.color = actor->sprite_color;

				zoom_indicators.append(tl);
				zoom_indicators.append(tr);
				zoom_indicators.append(br);
				zoom_indicators.append(bl);
			}
		}
		window.draw(zoom_indicators, &textures[1]);
	}
}

void PhysicsManager::draw_text(sf::RenderWindow& window, float zoom, std::vector<int> populations)
{
	sf::Text speed_text = sf::Text();
	speed_text.setFont(font);
	float warp = PhysicsChunk::time_warp;
	speed_text.setString("Speed: x" + round(warp, 4));
	speed_text.setCharacterSize(32);
	speed_text.setFillColor(sf::Color::White);
	speed_text.setPosition(0, 0);
	window.draw(speed_text);

	sf::Text zoom_text = sf::Text();
	zoom_text.setFont(font);
	zoom_text.setString("Zoom: " + round(zoom, 4));
	zoom_text.setCharacterSize(32);
	zoom_text.setFillColor(sf::Color::White);
	zoom_text.setPosition(0, 32 );
	window.draw(zoom_text);

	sf::Text energy_text = sf::Text();
	energy_text.setFont(font);
	energy_text.setString("World Energy: " + std::to_string((int)PlantActor::world_nutrition));
	energy_text.setCharacterSize(32);
	energy_text.setFillColor(sf::Color::White);
	energy_text.setPosition(0, 64);
	window.draw(energy_text);
	if (populations.size() > 0) {
		sf::Text plant_text = sf::Text();
		plant_text.setFont(font);
		plant_text.setString("Plant Population: " + std::to_string(populations[1]));
		plant_text.setCharacterSize(32);
		plant_text.setFillColor(sf::Color::White);
		plant_text.setPosition(0, 96);
		window.draw(plant_text);

		sf::Text vegan_text = sf::Text();
		vegan_text.setFont(font);
		vegan_text.setString("Vegan Population: " + std::to_string(populations[0]));
		vegan_text.setCharacterSize(32);
		vegan_text.setFillColor(sf::Color::White);
		vegan_text.setPosition(0, 128);
		window.draw(vegan_text);
	}
}

void PhysicsManager::draw_network(sf::RenderWindow& window, float zoom, Genome g, int x_offset, int y_offset, int network_display_width, int network_display_height)
{
	const auto last_layer = g.layers;
	std::unordered_map<int, std::pair<int, int>> layer_node_numbers{};
	int node_count = 0;
	int layer_count = 0;
	int biggest_layer = 0;
	for(const auto node : g.network)
	{
		if (layer_count != node.xPos)
		{
			layer_node_numbers[layer_count] = { 0, node_count };
			layer_count++;
			biggest_layer = node_count > biggest_layer ? node_count : biggest_layer;
			node_count = 0;
		}
		node_count++;
	}
	layer_node_numbers[layer_count] = { 0, node_count };
	biggest_layer = node_count > biggest_layer ? node_count : biggest_layer;

	std::unordered_map<int, std::pair<float, sf::Vector2f>> node_positions{};
	for (const auto& node : g.nodes)
	{
		node_count = layer_node_numbers[node.second->xPos].first;
		layer_node_numbers[node.second->xPos].first++;
		const auto layer_size = layer_node_numbers[node.second->xPos].second;
		node_positions[node.first] = { node.second->activatedSum, sf::Vector2f(((float)node.second->xPos/last_layer) * network_display_width + x_offset, ((node_count + 1.f)/(layer_size + 1.f)) * network_display_height + y_offset) };
	}


	int node_size = network_display_height / (biggest_layer * 2);
	{
		auto connection_sprites = sf::VertexArray(sf::Quads, g.connections.size() * 4);
		const auto cTX = textures[2].getSize().x;
		const auto cTY = textures[2].getSize().y;
		int connection_size = node_size / 10 > 5 ? node_size / 10 : 5;
		for(const auto& connection : g.connections)
		{
			sf::Vector2f pos1 = node_positions[connection.second->in].second;
			sf::Vector2f pos2 = node_positions[connection.second->out].second;
			sf::Color color = connection.second->disabled ?  sf::Color::Black : (connection.second->weight > 0 ? sf::Color::Blue : sf::Color::Red);
			int connection_width = abs(connection_size * connection.second->weight);
			connection_width = (connection_width < 1 ? 1 : connection_width);
			sf::Vertex tl;
			tl.position = pos1 + sf::Vector2f(0, (float)connection_width/2);
			tl.texCoords = sf::Vector2f(0, 0);
			tl.color = color;
			sf::Vertex tr;
			tr.position = pos2 + sf::Vector2f(0, (float)connection_width/2);
			tr.texCoords = sf::Vector2f(cTX, 0);
			tr.color = color;
			sf::Vertex br;
			br.position = pos2 + sf::Vector2f(0, -(float)connection_width/2);
			br.texCoords = sf::Vector2f(cTX, cTY);
			br.color = color;
			sf::Vertex bl;
			bl.position = pos1 + sf::Vector2f(0, -(float)connection_width/2);
			bl.texCoords = sf::Vector2f(0, cTY);
			bl.color = color;
			connection_sprites.append(tl);
			connection_sprites.append(tr);
			connection_sprites.append(br);
			connection_sprites.append(bl);
		}
		window.draw(connection_sprites, &textures[2]);
	}
	{
		auto node_sprites = sf::VertexArray(sf::Quads, g.network.size() * 4);
		const auto cTX = textures[1].getSize().x;
		const auto cTY = textures[1].getSize().y;
		for (const auto& node : node_positions)
		{
			sf::Vector2f pos = node.second.second;
			float color = (node.second.first + 1) * (255 / 2.f);
			color = color < 0 ? 0 : (color > 255 ? 255 : color);
			sf::Vertex tl;
			tl.position = pos + sf::Vector2f(-1 * node_size, -1 * node_size);
			tl.texCoords = sf::Vector2f(0, 0);
			tl.color = sf::Color((int)color, (int)color, (int)color);
			sf::Vertex tr;
			tr.position = pos + sf::Vector2f(node_size, -1 * node_size);
			tr.texCoords = sf::Vector2f(cTX, 0);
			tr.color = sf::Color((int)color, (int)color, (int)color);
			sf::Vertex br;
			br.position = pos + sf::Vector2f(node_size, node_size);
			br.texCoords = sf::Vector2f(cTX, cTY);
			br.color = sf::Color((int)color, (int)color, (int)color);
			sf::Vertex bl;
			bl.position = pos + sf::Vector2f(-1 * node_size, node_size);
			bl.texCoords = sf::Vector2f(0, cTY);
			bl.color = sf::Color((int)color, (int)color, (int)color);
			node_sprites.append(tl);
			node_sprites.append(tr);
			node_sprites.append(br);
			node_sprites.append(bl);
		}
		window.draw(node_sprites, &textures[1]);
	}
}

void PhysicsManager::draw(sf::RenderWindow& window) 
{
	
	std::lock_guard<std::mutex> lock(manager_mutex);
	auto zoom = window.getView().getSize().x / window.getDefaultView().getSize().x;
	float min_size = 3;
	std::vector<int> populations;
	//std::cout << circleTexture.getSize().x << ", " << circleTexture.getSize().y << std::endl;

	//int total = 0
	display_sprites(window, zoom, min_size, populations);
	{
		//std::cout << zoom << std::endl;
		sf::VertexArray zoom_indicators (sf::Quads);
		const auto cTX = textures[1].getSize().x;
		const auto cTY = textures[1].getSize().y;
		
		draw_zoom_indicators(window, zoom, min_size, zoom_indicators, cTX, cTY);
	}
	{
		auto saved_view = window.getView();
		window.setView(window.getDefaultView());
		draw_text(window, zoom, populations);
		auto viewport_width = window.getView().getSize().x;
		auto viewport_height = window.getView().getSize().y;
		if(selectedGenome.layers > 0)
			draw_network(window, zoom, selectedGenome, viewport_width / 3, 20, viewport_width * (2.f/3) - 20, viewport_height / 3);
		window.setView(saved_view);
	}
	//std::cout << circles.getVertexCount() << std::endl;
	
	//for (auto b : actors)
	//{
	//	b.second->draw(window);
	//}
	mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	if (velSelectedActor != nullptr) {
		drawLine(velSelectedActor->ball.pos, mousePos, sf::Color::Blue, window);
	}


}

std::string PhysicsManager::round(float f, int deci)
{
	return std::to_string(f).substr(0, (log10f(f) < 1 ? 1 : log10f(f)) + deci + 1);
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
	//if (selectedActor != nullptr) {
	//	std::lock_guard<std::mutex> lock(*selectedActor->mutex);
	//	selectedActor->ball.pos = mousePos;
	//}
	//auto end = std::chrono::high_resolution_clock::now();
	//std::cout << "selected_actor_check: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	

	//auto start = std::chrono::high_resolution_clock::now();
	auto exiting_actors = std::unordered_set<int>();
	CriticalMutex::all_frozen = true;
	std::vector<std::shared_ptr<Actor>> for_creation;
	std::unordered_set<int> for_deletion;
	for(auto i = chunk_threads.begin(); i != chunk_threads.end(); ++i)
	{
		std::lock_guard<std::mutex> lock(*i->second->mutex);
		if (!i->second->exiting_actors.empty()) {
			//std::cout << i->first << std::endl;
			exiting_actors.insert(i->second->exiting_actors.begin(), i->second->exiting_actors.end());
			i->second->exiting_actors.clear();
		}
		if (!i->second->for_deletion.empty()) {
			//std::cout << i->first << std::endl;
			for_deletion.insert(i->second->for_deletion.begin(), i->second->for_deletion.end());
			i->second->for_deletion.clear();
		}
		if(!i->second->for_creation.empty())
		{
			for_creation.insert(for_creation.end(), i->second->for_creation.begin(), i->second->for_creation.end());
			i->second->for_creation.clear();
		}
	}

	CriticalMutex::all_frozen = false;
	//auto end = std::chrono::high_resolution_clock::now();
	//std::cout << "populating exiting actors: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
	{
		std::lock_guard<std::mutex> lock(manager_mutex);
		for (auto& i : for_deletion)
		{
			if (actors.count(i) > 0) {
				texture_groups[actors.at(i)->sprite_id].erase(i);
				actors.erase(i);
			}
		}

		//TODO: PLANTACTOR CODE
		int spawn_rate = 1;

		for (int i = 0; i < spawn_rate; i++) {
			if (rand() % 10 == 0) {
				Ball temp = Ball();
				temp.pos = { (float)((rand() % (PhysicsCore::world_size.x / 1000)) * 1000 + rand() % 1000), (float)((rand() % (PhysicsCore::world_size.y / 1000)) * 1000 + rand() % 1000) };
				auto size = rand() % 500 + 200;
				//PlantActor::world_nutrition = PlantActor::world_nutrition - size;
				for_creation.push_back(std::make_shared<PlantActor>(temp, size));
			}
			
		}

		for (auto i : for_creation)
		{
			auto id = current_actor;
			current_actor++;
			i->setId(id);
			actors[id] = move(i);
			texture_groups[actors.at(id)->sprite_id].insert(id);

			exiting_actors.insert(id);
		}		
	}
	//auto start = std::chrono::high_resolution_clock::now();
	//if (!exiting_actors.empty())
	re_add_actor(exiting_actors);
	//auto end = std::chrono::high_resolution_clock::now();
	//std::cout << "re add actor: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "\n\n" << std::endl;
	Sleep(1);
}

std::array<int, 4> PhysicsManager::place_actor(const int b)
{
	const auto actor = actors.at(b);
	std::lock_guard<std::mutex> lock(*actor->mutex);
	if(actor->ball.pos.x < 0)
	{
		actor->ball.pos.x = 1;
	}
	if (actor->ball.pos.y < 0)
	{
		actor->ball.pos.y = 1;
	}
	if (actor->ball.pos.x > width)
	{
		actor->ball.pos.x = width - 1;
	}
	if (actor->ball.pos.y > height)
	{
		actor->ball.pos.y = height - 1;
	}

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

void PhysicsManager::add_actor(const std::vector<shared_ptr<Actor>>& added_actors)
{
	//CriticalMutex::all_frozen = true;
	auto chunk_add_map = std::unordered_map<int, std::queue<int>>();
	//for (auto i = chunk_threads.begin(); i != chunk_threads.end() ; ++i)
	//{
	//	const int n = i->first;
	//	//chunk_add_map.insert(n, );
	//}
	for (const auto& actor : added_actors)
	{
		actor->setId(current_actor);
		actors[current_actor] = actor;
		current_actor++;
		texture_groups[actor->sprite_id].insert(actor->getId());
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
				chunk_threads[i.first]->limbo_list[j].push_back(actors.at(chunk_add_map[j].front()));
				chunk_add_map[j].pop();
			}			
		}
	}
	//TODO: add genomes to population
	//population.
	//CriticalMutex::all_frozen = false;
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
		if (actors.count(actor) > 0) {
			auto list = place_actor(actors.at(actor)->getId());
			for (auto i : list)
			{
				if (i != -1)
				{
					if (chunk_add_map.count(i) <= 0)
						chunk_add_map[i] = std::queue<int>();
					chunk_add_map[i].push(actors.at(actor)->getId());
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
			thread_add_map[thread][i->first].push_back(actors.at(i->second.front()));
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



int PhysicsManager::get_chunk(sf::Vector2f vec) const
{
	const auto chunk_x = static_cast<int>(vec.x) / chunk_width;
	const auto chunk_y = static_cast<int>(vec.y) / chunk_height;
	return chunk_x + (chunk_y * 100000);
}

void PhysicsManager::select_actor()
{
	float min_dist = 1000000000;
	shared_ptr<Actor> min_dist_actor;
	for (auto b : actors) {
		if (b.second != nullptr) {
			const auto dist = sqrt(Ball::magsq(mousePos - b.second->ball.pos));
			if (dist < min_dist) {
				min_dist = dist;
				min_dist_actor = b.second;
			}
		}
	}
	selectedActor = min_dist_actor;
	min_dist_actor->selected = true;
	global_selected_actor = selectedActor->getId();
	if(!selectedActor->brainless)
	{
		selectedGenome = selectedActor->genome;
	}
	return;
}

void PhysicsManager::deselect_actor()
{
	if (selectedActor != nullptr) {
		selectedActor->ball.active = true;
		selectedActor->selected = false;
	}
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


