#pragma once
#include <memory>
#include <vector>
#include <SFML/System/Vector2.hpp>


class Actor;

class Ray
{
public:
	static bool hit(sf::Vector2f source, float angle, float range, sf::Vector2f point, float rad);
	static int hit(sf::Vector2f source, float angle, float range, std::vector<std::shared_ptr<Actor>>& actors);
};
