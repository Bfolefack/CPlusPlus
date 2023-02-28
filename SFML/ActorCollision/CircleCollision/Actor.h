#pragma once
#include <unordered_set>

#include "Ball.h"

class Actor
{
protected:
	int id;
public:
	//virtual ~Actor() = default;
	bool for_deletion;
	std::vector<std::shared_ptr<Actor>> for_creation;
	Ball ball;
	int actor_type;
	int sprite_id;
	sf::Color sprite_color;
	float facing;
	float sight_range;
	std::shared_ptr<std::mutex> mutex;
	std::vector<float> ray_angles;
	std::vector<std::shared_ptr<Actor>> sighted_actors;
	static sf::Vector2i world_size;

	Actor();
	Actor(const Ball& b);
	sf::Vector2f avoid_walls() const;
	void ray_cast(const std::unordered_map<int, std::shared_ptr<Actor>>& actors);
	void setId(int i);
	int getId() const;
	bool hit(sf::Vector2f source, float angle, float range, sf::Vector2f point, float rad) const;
	int hit(sf::Vector2f source, float angle, float range,
	        const std::unordered_map<int, std::shared_ptr<Actor>>& actors) const;

	virtual void update() = 0;
	virtual void draw() = 0;
};
