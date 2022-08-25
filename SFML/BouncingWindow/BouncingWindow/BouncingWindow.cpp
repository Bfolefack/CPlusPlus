#include "BouncingWindow.h"
#include "SFML/Window.hpp"
#include <Windows.h>
#include <chrono>
#include <iostream>
#include "BouncingWindow.h"

using sf::Window;

BouncingWindow::BouncingWindow(int velMag, int s) {
	srand((std::chrono::system_clock::now().time_since_epoch()).count());
	size = s;
	xVel = (rand() % (velMag * 2) - velMag);
	yVel = (rand() % (velMag * 2) - velMag);

	window.create(sf::VideoMode(200, 200), "Lol Idiot");
	window.setPosition(sf::Vector2i(rand() % 1920, rand() % 1080));
	mouseInside = false;
}

BouncingWindow::BouncingWindow(const BouncingWindow& old) {
	size = old.size;
	xVel = old.xVel;
	yVel = old.yVel;

	window.create(sf::VideoMode(size, size), "Lol Idiot");
	window.setPosition(old.window.getPosition());
	mouseInside = false;
};

bool BouncingWindow::bounce() {
	//printf("bouncing");
	sf::Event event;
	bool out = false;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			out = true;
		if (event.type == sf::Event::MouseEntered) {
			//printf("The Mouse Has Entered\n");
			mouseInside = true;
		}
		if (event.type == sf::Event::MouseLeft) {
			//printf("The Mouse Has Left\n");
			mouseInside = false;
		}
		if (event.type == sf::Event::MouseButtonPressed) {
			printf("You Got Me!!!");
			out = true;
		}
		if (event.type == sf::Event::KeyPressed) {
			window.close();
		}
	}
	sf::Vector2i position = window.getPosition();
	int xPos = position.x;
	int yPos = position.y;
	if (xPos > 1920 - size) {
		xVel *= -1;
		xPos = 1920 - size;
	}
	if (xPos < 0)
		xVel *= -1;
	if (yPos > (1080 - size)) {
		yVel *= -1;
		yPos = 1080 - size;
	}
	if (yPos < 0)
		yVel *= -1;
	window.setPosition(sf::Vector2i(xVel + xPos, yVel + yPos));
	return out;
}