#pragma once
#include "Ball.h"

class Actor
{
protected:
	int id;
public:
	//virtual ~Actor() = default;
	bool for_deletion;
	Ball ball;
	int sprite_id;
	int facing;
	int sight_range;
	std::shared_ptr<std::mutex> mutex;
	std::vector<float> ray_angles;
	std::vector<std::shared_ptr<Actor>> sighted_actors;
	Actor();
	Actor(const Ball& b);
	void ray_cast(std::vector<std::shared_ptr<Actor>>& actors);
	void setId(int i);
	int getId() const;

	virtual void update() = 0;
	virtual void draw() = 0;
};
