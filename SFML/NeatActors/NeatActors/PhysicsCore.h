#pragma once
#include <unordered_set>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include "Ball.h"

class PhysicsCore
{
public:
	int id;
	//virtual ~PhysicsCore() = default;
	bool for_deletion;
	bool selected = false;
	std::vector<std::shared_ptr<PhysicsCore>> for_creation;
	Ball ball;
	int actor_type;
	int sprite_id;
	sf::Color sprite_color;
	float facing;
	float sight_range;
	std::shared_ptr<std::mutex> mutex;
	static sf::Vector2i world_size;

	PhysicsCore();
	PhysicsCore(const Ball& b);
	sf::Vector2f avoid_walls() const;
	

	void setId(int i);
	int getId() const;
	bool hit(sf::Vector2f source, float angle, float range, sf::Vector2f point, float rad) const;
	std::pair<int, float> hit(sf::Vector2f source, float angle, float range,
	                          const std::unordered_map<int, std::shared_ptr<PhysicsCore>>& actors) const;

	virtual void update(float delta_time) = 0;
};
