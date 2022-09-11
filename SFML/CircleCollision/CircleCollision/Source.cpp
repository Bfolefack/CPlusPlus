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
	const std::unique_ptr<RenderWindow> window(new RenderWindow(sf::VideoMode(1900, 1000), "Balls"));
	window->setFramerateLimit(144);
	auto p = PhysicsManager(1900, 1000, 100, 100);
	for (int i = 0; i < 500; i++) {
		Ball b(i, 10, rand() % (window->getSize().x), rand() % (window->getSize().y), 0.5f * (rand() % 2 - 0.5), 0.5f * (rand() % 2 - 0.5), 10, 0.9f);
		p.add_ball(b);
	}
	float delta_time = 1.f/ 144;
	while (window->isOpen()) {
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window->close();
			}	
			//if (event.type == sf::Event::MouseButtonPressed) {
			//	if (event.mouseButton.button == sf::Mouse::Left) {
			//		p.select_ball();
			//	}
			//	else if (event.mouseButton.button == sf::Mouse::Right) {
			//		p.vel_select_ball();
			//	}
			//}
			//if (event.type == sf::Event::MouseButtonReleased) {
			//	if (event.mouseButton.button == sf::Mouse::Left) {
			//		p.deselect_ball();
			//	}
			//	else if (event.mouseButton.button == sf::Mouse::Right) {
			//		p.vel_deselect_ball();
			//	}
			//}
		}
		p.update();
		p.draw(*window);
		//p.resolveCollisions();
		window->display();
		window->clear(sf::Color::Black);
	}
	
	return 0;
}