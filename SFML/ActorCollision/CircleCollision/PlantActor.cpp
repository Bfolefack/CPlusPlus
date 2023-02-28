#include "PlantActor.h"

#include <utility>

std::unordered_map<int, std::unordered_map<int, int>> PlantActor::nutrition_map = std::unordered_map<int, std::unordered_map<int, int>>();
std::mutex PlantActor::nutrition_map_mutex;

void PlantActor::draw() {}

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
	growth_rate = gr * (1 + (rand() % 1000 - 500) / 10000.f * mr);
	if (growth_rate < 100)
		growth_rate = 100;
	if (growth_rate > 10000)
		growth_rate = 10000;

	max_growth = mg * (1 + (rand() % 1000 - 500) / 10000.f * mr);
	if (max_growth < 10)
		max_growth = 10;
	if (max_growth > 200)
		max_growth = 200;

	child_num = cn * (1 + (rand() % 1000 - 500) / 10000.f * mr);
	if (child_num < 2)
		child_num = 2;

	ejection_force = ef * (1 + (rand() % 1000 - 500) / 10000.f * mr);
	if (ejection_force < 10)
		ejection_force = 10;
	if(ejection_force > 500)
		ejection_force = 500;

	elasticity = elas * (1 + (rand() % 1000 - 500) / 10000.f * mr);
	if (elasticity < 0.01f)
		elasticity = 0.01f;
	if (elasticity > 0.99f)
		elasticity = 0.99f;

	friction = fric * (1 + (rand() % 1000 - 500) / 10000.f * mr);
	if (friction < 0.f)
		friction = 0.f;
	if (friction > 0.2f)
		friction = 0.2f;

	max_age = ma * (1 + (rand() % 1000 - 500) / 10000.f * mr);
	if (max_age > 100000)
		max_age = 100000;
	if (max_age < 1000)
		max_age = 1000;

	max_growth_step = mgs * (1 + (rand() % 1000 - 500) / 10000.f * mgs);
	if(max_growth_step < 1)
	{
		max_growth_step = 1;
	}

	mutation_rate = mr * (1 + (rand() % 1000 - 500) / 10000.f * mr);

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

PlantActor::PlantActor(Ball& b) : Actor()
{
	ball = b;


	growth_rate = rand() % 9900 + 100;
	max_growth = rand() % 80 + 20;
	max_growth_step = 1 + rand() % 20;
	child_num = 2 + rand() % 10;
	ejection_force = 1 + rand() % 100;
	elasticity = 1 - (rand() % 1000) / 1000.f;
	friction = (rand() % 100)/100.f;
	mutation_rate = .1f;
	size = 1000;
	max_age = 20000;
	age = 0;
	for_deletion = false;
	ball.always_active = false;
	ball.active = true;
	ball.friction = 0.01f;
	b.rad = sqrtf(size);
	sprite_id = 1;
	sprite_color = sf::Color::Green;
	sight_range = 100;
}


void PlantActor::update()
{
	std::lock_guard<std::mutex> lock(*mutex);
	if (rand() % growth_rate == 0 && !for_deletion) {
		int growth = max_growth_step;
		if (growth > nutrition_map[(int)(ball.pos.x / nutrition_map_size)][(int)(ball.pos.y / nutrition_map_size)]) {
			size -= growth;
			nutrition_map[(int)(ball.pos.x / nutrition_map_size)][(int)(ball.pos.y / nutrition_map_size)] += growth;
			ball.active = true;
			if(size < 5)
			{
				nutrition_map[(int)(ball.pos.x / nutrition_map_size)][(int)(ball.pos.y / nutrition_map_size)] += size;
				for_deletion = true;
			}
			return;
		}
		else {
			nutrition_map[(int)(ball.pos.x / nutrition_map_size)][(int)(ball.pos.y / nutrition_map_size)] -= growth;
			size += growth;
			ball.active = true;
		}
	}
	if(size > max_growth && !for_deletion)
	{
		const int num_children = child_num > size / 5 ? size / 5 : child_num;
		if (num_children > 2) {
			for (int i = 1; i < num_children; i++)
			{
				auto temp = PlantActor(growth_rate, max_growth, child_num, ejection_force, elasticity, friction, mutation_rate, max_growth_step, max_age);
				temp.size = size / num_children;
				nutrition_map[(int)(ball.pos.x / nutrition_map_size)][(int)(ball.pos.y / nutrition_map_size)] -= temp.size;
				temp.ball.rad = size;
				temp.ball.pos = { ball.pos.x + rand() % 10 - 5, ball.pos.y + rand() % 10 - 5 };
				const auto angle = ((rand() % 6284) - 3142) / 1000;
				temp.ball.vel = { cosf(angle) * ejection_force, sinf(angle) * ejection_force };
				for_creation.push_back(std::make_shared<PlantActor>(std::move(temp)));
			}
		}
		nutrition_map[(int)(ball.pos.x / nutrition_map_size)][(int)(ball.pos.y / nutrition_map_size)] += size;
		for_deletion = true;
	}
	age++;
	if (age > max_age)
	{
		nutrition_map[(int)(ball.pos.x / nutrition_map_size)][(int)(ball.pos.y / nutrition_map_size)] += size;
		for_deletion = true;
	}
	ball.rad = size;
}
