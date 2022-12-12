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
	growth_rate = 1000;
	max_growth = 50;
	max_growth_step = 5;
	child_num = 5;
	ejection_force = 25;
	elasticity = 0.9f;
	friction = 0.05f;
	mutation_rate = 1.f;
	max_age = 5000;
	age = 0;
	for_deletion = false;
	ball.active = true;
	ball.always_active = false;

	ball.friction = friction;
	ball.elasticity = elasticity;
	size = rand() % 20 + 5;
	sprite_id = 1;
	sprite_color = sf::Color::Green;
	sight_range = 100;
}

PlantActor::PlantActor(int gr, int mg, int cn, float ef, float elas, float fric, float mr, int mgs, int ma) : Actor()
{
	size = -1;
	projected_growth = 0;

	growth_rate = gr;
	max_growth = mg;
	child_num = cn;
	ejection_force = ef;
	elasticity = elas;
	friction = fric;
	mutation_rate = mr;
	max_growth_step = mgs;
	max_age = ma;

	age = 0;
	for_deletion = false;
	ball.active = true;
	ball.always_active = false;

	ball.friction = friction;
	ball.elasticity = elasticity;

	sprite_id = 1;
	sprite_color = sf::Color::Green;
	sight_range = 100;
}

void PlantActor::mutate()
{
	size = -1;
	growth_rate = growth_rate * (1 + (rand() % 1000 - 500) / 10000.f * mutation_rate);
	if (growth_rate < 500)
		growth_rate = 500;
	if (growth_rate > 100000)
		growth_rate = 100000;

	max_growth = max_growth * (1 + (rand() % 1000 - 500) / 10000.f * mutation_rate);
	if (max_growth < 10)
		max_growth = 10;
	if (max_growth > 600)
		max_growth = 600;

	child_num = child_num * (1 + (rand() % 1000 - 500) / 10000.f * mutation_rate);
	if (child_num < 2)
		child_num = 2;

	ejection_force = ejection_force * (1 + (rand() % 1000 - 500) / 10000.f * mutation_rate);
	if (ejection_force < 10)
		ejection_force = 10;
	if (ejection_force > 200)
		ejection_force = 200;

	elasticity = elasticity * (1 + (rand() % 1000 - 500) / 10000.f * mutation_rate);
	if (elasticity < 0.01f)
		elasticity = 0.01f;
	if (elasticity > 0.99f)
		elasticity = 0.99f;

	friction = friction * (1 + (rand() % 1000 - 500) / 10000.f * mutation_rate);
	if (friction < 0.f)
		friction = 0.f;
	if (friction > 0.2f)
		friction = 0.2f;

	max_age = max_age * (1 + (rand() % 1000 - 500) / 10000.f * mutation_rate);
	if (max_age > 100000)
		max_age = 100000;
	if (max_age < 1000)
		max_age = 1000;

	max_growth_step = max_growth_step * (1 + (rand() % 1000 - 500) / 10000.f * mutation_rate);
	if (max_growth_step < 1)
	{
		max_growth_step = 1;
	}

	mutation_rate = mutation_rate * (1 + (rand() % 1000 - 500) / 10000.f * mutation_rate);

	
}

PlantActor::PlantActor(Ball& b) : Actor()
{
	ball = b;

	projected_growth = 0;
	growth_rate = rand() % 2200 + 100;
	max_growth = rand() % 500 + 100;
	max_growth_step = 20 + rand() % 280;
	child_num = 2 + rand() % 10;
	ejection_force = 25 + rand() % 100;
	elasticity = 1 - (rand() % 1000) / 1000.f;
	friction = (rand() % 500)/10000.f;
	mutation_rate = .1f;
	size = rand() % 600 + 500;
	max_age = 20000;
	age = 0;
	for_deletion = false;
	ball.always_active = false;
	ball.active = true;
	ball.friction = 0.01f;

	b.rad = sqrtf(size * 100);
	sprite_id = 1;
	sprite_color = sf::Color::Green;
	sight_range = 100;
	world_nutrition = world_nutrition - size;
}


void PlantActor::update(float delta_time)
{
	std::lock_guard<std::mutex> lock(*mutex);
	if (rand() % (int)(growth_rate/delta_time) == 0 && !for_deletion && abs(projected_growth) < max_growth_step) {
		int growth = max_growth_step;
		if (growth > world_nutrition) {
			projected_growth -= growth;
			world_nutrition = world_nutrition + growth;
			
				
			
		}
		else {
			world_nutrition = world_nutrition - growth;
			projected_growth += growth;
			
				
			
		}
	}
	if (size + projected_growth < 150)
	{
		world_nutrition =  world_nutrition + size + projected_growth;
		for_deletion = true;
		return;
	}
	if(size > max_growth && !for_deletion)
	{
		const int num_children = child_num > (size / 150) + 1 ? size / 150 : child_num;
		if (num_children > 2) {
			for (int i = 1; i < num_children; i++)
			{
				auto temp = PlantActor(growth_rate, max_growth, child_num, ejection_force, elasticity, friction, mutation_rate, max_growth_step, max_age);
				temp.size = size / num_children;
				world_nutrition = world_nutrition - temp.size;
				
				temp.ball.pos = { ball.pos.x + rand() % 10 - 5, ball.pos.y + rand() % 10 - 5 };
				const auto angle = ((rand() % 6284) - 3142) / 1000;
				temp.ball.vel = { cosf(angle) * ejection_force, sinf(angle) * ejection_force };
				for_creation.push_back(std::make_shared<PlantActor>(std::move(temp)));

				
					
			}
		}
		world_nutrition = world_nutrition + size + projected_growth;
		for_deletion = true;

		
			
	}
	if(projected_growth != 0.f)
	{
		if(projected_growth > 0)
		{
			size += delta_time;
			projected_growth -= delta_time;
			ball.active = true;
		} else
		{
			size -= delta_time;
			projected_growth += delta_time;
			ball.active = true;
		}
		if(abs(projected_growth) < 1)
		{
			world_nutrition = world_nutrition + projected_growth;
			projected_growth = 0.f;
		}
	}
	age++;
	if (age > max_age)
	{
		world_nutrition = world_nutrition + size + projected_growth;
		for_deletion = true;

		
			
	}

	if(isnan((float)world_nutrition))
	{
		std::cout << "Nanned";
	}
	ball.rad = sqrt(size * 100);
	ball.mass = 2 * 3.14 * ball.rad * ball.rad * 0.1;
}
