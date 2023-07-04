#include "PlantActor.h"

#include <iostream>
#include <utility>

std::atomic<float> PlantActor::world_nutrition;


void PlantActor::ray_cast(const std::unordered_map<int, std::shared_ptr<Actor>>& actors)
{
	return;
}

PlantActor::PlantActor() : Actor()
{
	//int growth_rate;
	//int max_growth;
	//int child_num;
	//float ejection_force;
	//float elasticity;
	//float friction;
	//float mutation_rate;

	for_deletion = false;
	ball.active = true;
	ball.always_active = false;

	projected_growth = 0;

	ball.friction = 0.05;
	ball.elasticity = 0.9;
	size = rand() % 20 + 5;
	sprite_id = 1;
	sprite_color = sf::Color::Green;
	sight_range = 0;
}

PlantActor::PlantActor(Ball& b) : Actor()
{
	ball = b;

	size = rand() % 600 + 100;
	for_deletion = false;
	ball.always_active = false;
	ball.active = true;
	ball.friction = 0.01f;
	ball.elasticity = 0.9f;
	projected_growth = 0;

	b.rad = sqrtf(size * 10);
	b.mass = 6.28f * b.rad * b.rad * 0.1f;
	sprite_id = 1;
	sprite_color = sf::Color::Green;
	sight_range = 100;
}


PlantActor::PlantActor(Ball& b, int s) : Actor()
{
	ball = b;

	size = s;
	for_deletion = false;
	ball.always_active = false;
	ball.active = true;
	ball.friction = 0.01f;
	ball.elasticity = 0.9f;
	projected_growth = 0;

	b.rad = sqrtf(size * 10);
	b.mass = 6.28f * b.rad * b.rad * 0.1f;
	sprite_id = 1;
	sprite_color = sf::Color::Green;
	sight_range = 100;
}

void PlantActor::update(float delta_time)
{
	std::lock_guard<std::mutex> lock(*mutex);


	//world_nutrition = -1000;
	

	if (projected_growth != 0.f)
	{
		if (projected_growth > 0)
		{
			size += delta_time;
			projected_growth -= delta_time;
			ball.active = true;
		}
		else
		{
			size -= delta_time;
			projected_growth += delta_time;
			ball.active = true;
		}
		if(abs(projected_growth) < 1)
		{
			//world_nutrition = world_nutrition + projected_growth;
			projected_growth = 0.f;
		}
	}

	if (size + projected_growth < 10 && !for_deletion)
	{
		for_deletion = true;
		//world_nutrition = world_nutrition + size + projected_growth;
	}


	ball.rad = sqrt(size * 10);
	ball.mass = 2 * 3.14 * ball.rad * ball.rad * 0.1;
}
