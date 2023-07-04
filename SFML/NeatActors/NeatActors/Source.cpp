#include "SFML/Graphics.hpp"
#include "SFML/window.hpp"
#include "PhysicsCore.h"
#include "PhysicsChunk.h"
#include <random>
#include <iostream>
#include <chrono>
#include <Windows.h>
#include <thread>

#include "BasicActor.h"
#include "PhysicsManager.h"
#include "PlantActor.h"
#include "VeganActor.h"
#include "Genome.h"
#include "SmartVeganActor.h"

using sf::RenderWindow;
int world_width = 25000;
int world_height = 25000;

int main() {
	srand((std::chrono::system_clock::now().time_since_epoch()).count());
	//RenderWindow real_window(sf::VideoMode(1900, 1000), "Actors");
	const std::unique_ptr<RenderWindow> window(new RenderWindow(sf::VideoMode(1900, 1000), "Actors"));
	
	window->setFramerateLimit(144);
	sf::View view = window->getDefaultView();

	PhysicsCore::world_size = { world_width, world_height };
	//PlantActor::world_nutrition = 100000;

	std::vector<shared_ptr<Actor>> actors = {};
	for (int i = 0; i < 100; i++) {
		std::cout << "VeganActor: " << i << std::endl;
		const Ball b = Ball(5, (rand() % (world_width / 1000)) * 1000 + rand() % 1000, (rand() % (world_height / 1000)) * 1000 + rand() % 1000, 100, 0.9f, 0.03f);
		//for(int i = 0; i < 10; i++)
		actors.push_back(std::make_shared<SmartVeganActor>(SmartVeganActor(b)));
		//actors.push_back(std::make_shared<VeganActor>(VeganActor(b)));

	}
	for (int i = 0; i < 1000; i++) {
		std::cout << "PlantActor: " << i << std::endl;
		Ball b = Ball(5, (rand() % (world_width/1000)) * 1000 + rand() % 1000, (rand() % (world_height / 1000)) * 1000 + rand() % 1000, 100, 0.9f, 0.03f);
		//for(int i = 0; i < 10; i++)
		actors.push_back(std::make_shared<PlantActor>(PlantActor(b)));
	}

	
	

	PhysicsManager p(world_width, world_height, 500, 500);
	p.add_actor(actors);
	actors.clear();


	auto update_thread = std::thread(&PhysicsManager::perpetual_update, &p);
	//auto draw_thread = std::thread(&PhysicsManager::perpetual_draw, &p, std::ref(* window));
	update_thread.detach();
	//draw_thread.detach();
	int frame_count = 0;
	while (window->isOpen()) {
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window->close();
				return 0;
			}
			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					p.select_actor();
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					p.vel_select_actor();
				}
			}
			if (event.type == sf::Event::MouseButtonReleased) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					p.deselect_actor();
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					p.vel_deselect_actor();
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				view.move(0, -10 * (window->getView().getSize().y / window->getSize().y));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				view.move(-10 * (window->getView().getSize().x / window->getSize().x), 0);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				view.move(0, 10 * (window->getView().getSize().y / window->getSize().y));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				view.move(10 * (window->getView().getSize().x / window->getSize().x), 0);
			}


			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Period))
			{
				if (PhysicsChunk::time_warp < 1.f)
					PhysicsChunk::time_warp *= 1.41421356237;
				if(PhysicsChunk::time_warp < 30.f)
					PhysicsChunk::time_warp += 0.5;
				continue;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Comma))
			{
				if (PhysicsChunk::time_warp > 1.f)
					PhysicsChunk::time_warp -= 0.5;
				else if (PhysicsChunk::time_warp > 0.05f)
					PhysicsChunk::time_warp /= 1.41421356237;
				continue;
			}


			if(event.type == sf::Event::MouseWheelScrolled)
			{
				//std::cout << event.mouseWheelScroll.delta << std::endl;
				view.zoom(1 + 0.1f * event.mouseWheelScroll.delta);
			}
			if (event.type == sf::Event::Resized)
			{
				// update the view to the new size of the window
				sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
				window->setView(sf::View(visibleArea));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				view = window->getDefaultView();
				PhysicsChunk::time_warp = 1.f;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
			{
				auto mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
				//p.add_actor(PhysicsCore(0, 5 + (rand() % 5) * (rand() % 5) * (rand() % 5), mousePos.x, mousePos.y, 0.5f * (rand() % 2 - 0.5), 0.5f * (rand() % 2 - 0.5), 10, .99f));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				return 0;
			}
			
			window->setView(view);
			
		}
		if (PhysicsChunk::time_warp > 1.f && PhysicsChunk::time_warp < 2.f)
			PhysicsChunk::time_warp = 1;
		//Sleep(1);
		//auto start = std::chrono::high_resolution_clock::now();
		////p.update();
		//auto end = std::chrono::high_resolution_clock::now();
		//std::cout << "Update: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		//auto start = std::chrono::high_resolution_clock::now();
		p.draw(*window);
		//auto end = std::chrono::high_resolution_clock::now();
		//std::cout << "Draw: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		window->display();
		window->clear(sf::Color(125, 125, 125));
		//Sleep(100);
		frame_count++;
	}
	
	return 0;
}