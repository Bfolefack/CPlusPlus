#include "Drawing.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Vertex.hpp>

void Drawing::draw_line(sf::Vector2f point1, sf::Vector2f point2, sf::Color color, sf::RenderWindow& window) {
	sf::Vertex line[2];
	line[0].position = point1;
	line[1].position = point2;
	line[0].color = color;
	line[1].color = color;
	window.draw(line, 2, sf::Lines);
}