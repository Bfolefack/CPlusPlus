#include "SFML/Graphics.hpp"
#include "SFML/window.hpp"
#include "Actor.h"
#include "PhysicsChunk.h"
#include <random>
#include <iostream>
#include <chrono>
#include <Windows.h>
#include <thread>

#include "BasicActor.h"
#include "FlockActor.h"
#include "PhysicsManager.h"
#include "PlantActor.h"

using sf::RenderWindow;


int main() {
	srand((std::chrono::system_clock::now().time_since_epoch()).count());
	//RenderWindow real_window(sf::VideoMode(1900, 1000), "Actors");
	const std::unique_ptr<RenderWindow> window(new RenderWindow(sf::VideoMode(1050, 1050), "Actors"));
	
	window->setFramerateLimit(144);
	sf::View view = window->getDefaultView();

	int world_width = 50000;
	int world_height = 50000;

	PlantActor::nutrition_map = std::unordered_map<int, std::unordered_map<int, int>>();
	for (int i = 0; i < world_width; i += PlantActor::nutrition_map_size)
	{
		for (int j = 0; j < world_height; j += PlantActor::nutrition_map_size)
		{
			PlantActor::nutrition_map[i/PlantActor::nutrition_map_size][j/PlantActor::nutrition_map_size] = PlantActor::nutrition_map_default;
		}
	}

	std::vector<shared_ptr<Actor>> actors = {};
	for (int i = 0; i < 10; i++) {
		std::cout << "Actor: " << i << std::endl;
		Actor::world_size = { world_width, world_height };
		Ball b = Ball(5, (rand() % (world_width/1000)) * 1000 + rand() % 1000, (rand() % (world_height / 1000)) * 1000 + rand() % 1000, 100, 0.9f, 0.03f);
		//for(int i = 0; i < 10; i++)
		actors.push_back(std::make_shared<PlantActor>(PlantActor(b)));
	}


	PhysicsManager p(world_width, world_height, 200, 200);
	p.add_actor(actors);


	auto update_thread = std::thread(&PhysicsManager::perpetual_update, &p);
	//auto draw_thread = std::thread(&PhysicsManager::perpetual_draw, &p, std::ref(* window));
	update_thread.detach();
	//draw_thread.detach();
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
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
			{
				auto mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
				//p.add_actor(Actor(0, 5 + (rand() % 5) * (rand() % 5) * (rand() % 5), mousePos.x, mousePos.y, 0.5f * (rand() % 2 - 0.5), 0.5f * (rand() % 2 - 0.5), 10, .99f));
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				return 0;
			}
			
			window->setView(view);
			
		}
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
	}
	
	return 0;
}