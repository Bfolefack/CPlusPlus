#pragma once
#include "SFML/Graphics.hpp"

class BouncingWindow {
public:
	sf::Window window;
	int xVel;
	int yVel;
	int size;
	bool mouseInside;
	
	BouncingWindow(int v, int s);
	BouncingWindow(const BouncingWindow &old);
	bool bounce();
};