#include "SFML/Graphics.hpp"
#include "SFML/window.hpp"
#include "Ball.h"
#include "PhysicsChunk.h"
#include <random>
#include <iostream>
#include <chrono>
#include <Windows.h>
#include <thread>

#include "PhysicsManager.h"

using sf::RenderWindow;

int main() {
	srand((std::chrono::system_clock::now().time_since_epoch()).count());
	//RenderWindow real_window(sf::VideoMode(1900, 1000), "Balls");
	const std::unique_ptr<RenderWindow> window(new RenderWindow(sf::VideoMode(1050, 1050), "Balls"));
	
	window->setFramerateLimit(144);
	sf::View view = window->getDefaultView();

	std::vector<shared_ptr<Ball>> balls = {};
	for (int i = 0; i < 5000; i++) {
		std::cout << "Ball: " << i << std::endl;
		Ball b(i, 25, rand() % (10000), rand() % (10000), 0.5f * (rand() % 2 - 0.5), 0.5f * (rand() % 2 - 0.5), 10, 0.9f);
		balls.push_back(std::make_shared<Ball>(b));
	}


	PhysicsManager p(10000, 10000, 200, 200);
	p.add_ball(balls);

	float delta_time = 1.f/ 144;
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
					p.select_ball();
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					p.vel_select_ball();
				}
			}
			if (event.type == sf::Event::MouseButtonReleased) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					p.deselect_ball();
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					p.vel_deselect_ball();
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
			window->setView(view);
		}
		Sleep(1);
		//auto start = std::chrono::high_resolution_clock::now();
		////p.update();
		//auto end = std::chrono::high_resolution_clock::now();
		//std::cout << "Update: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		//auto start = std::chrono::high_resolution_clock::now();
		p.draw(*window);
		//auto end = std::chrono::high_resolution_clock::now();
		//std::cout << "Draw: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
		window->display();
		window->clear(sf::Color::White);
		//Sleep(100);
	}
	
	return 0;
}