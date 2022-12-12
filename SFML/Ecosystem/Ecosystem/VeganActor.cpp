#include "VeganActor.h"

#include <iostream>

#include "PlantActor.h"



VeganActor::VeganActor(const Ball& b) : Actor(b)
{
	ball = b;
	ball.active = true;
	ball.always_active = true;
	//ball.friction = 0.025f;
	for (float f = -3.14159265358979323846f; f < 3.14159265358979323846f; f += 3.14159265358979323846f / 15)
	{
		ray_angles.push_back(f);
		sighted_actors.push_back(nullptr);
	}

	facing = ((rand() % 6284) - 3142) / 1000.f;
	ball.vel = { cos(facing), sin(facing) };
	ball.vel *= 0.05f;
	ball.friction = 0.01f;
	wander_angle = facing;

	max_speed = rand_range(10, 50);
	max_turn = rand_range(10, 500)/10000.f;
	sight_range = rand_range(100, 1000);
	turn_countdown = 0;
	size = rand_range(250, 750);
	energy = size;
	child_starting_energy = 100;
	child_energy_threshold = rand_range(50, 150)/100.f;
	mutation_rate = rand_range(10, 100)/1000.f;
	mutate();
}




VeganActor::VeganActor(const VeganActor& parent) : Actor(parent.ball)
{
	ball = parent.ball;
	ball.active = true;
	ball.always_active = true;
	//ball.friction = 0.025f;
	for (float f = -3.14159265358979323846f; f < 3.14159265358979323846f; f += 3.14159265358979323846f / 15)
	{
		ray_angles.push_back(f);
		sighted_actors.push_back(nullptr);
	}

	facing = ((rand() % 6284) - 3142) / 1000.f;
	ball.vel = { cos(facing), sin(facing) };
	ball.vel *= 0.05f;
	ball.friction = 0.01f;


	desired_vector = { 0, 0 };
	turn_countdown = 0;
	wander_angle = facing;

	sprite_id = parent.sprite_id;

	max_turn = parent.max_turn;
	max_speed = parent.max_speed;
	mutation_rate = parent.mutation_rate;
	sight_range = parent.sight_range;
	size = parent.size;
	energy = parent.child_starting_energy;
	child_starting_energy = parent.child_starting_energy;
	child_energy_threshold = parent.child_energy_threshold;
	energy = 0;
	mutate();
}

void VeganActor::mutate()
{
	max_speed = mutate(max_speed);
	if (max_speed < 1)
		max_speed = 1;

	max_turn = mutate(max_turn);
	if (max_turn < 0.001f)
		max_turn = 0.001f;
	size = mutate(size);
	if (size < 20)
		size = 20;
	sight_range = mutate(sight_range);
	if (sight_range < 10)
		sight_range = 10;
	mutation_rate = mutate(mutation_rate);
	child_starting_energy = mutate(child_starting_energy);
	child_energy_threshold = mutate(child_energy_threshold);



	energy = child_starting_energy;
	ball.rad = sqrtf(size);
	ball.mass = 2 * 3.14159 * ball.rad * ball.rad;

	ball.max_vel = max_speed * 2;
	ball.sqrt_max_vel = sqrtf(ball.max_vel);

	sprite_id = 0;
	sprite_color = sf::Color(max_speed * 7, size / 3, sight_range / 4);
}

float VeganActor::mutate(float num) const
{
	auto mutagen = (1 + (rand() % 1000 - 500) / (1000.f/mutation_rate));
	return num * mutagen;
}

int VeganActor::rand_range(int min, int max)
{
	return (rand() % (max - min)) + min;
}


void VeganActor::loop()
{
	if (ball.pos.x < ball.rad * 2)
	{
		ball.pos.x = world_size.x - 1;
	}
	if (ball.pos.y < ball.rad * 2)
	{
		ball.pos.y = world_size.y - 1;
	}
	if (ball.pos.x > world_size.x - ball.rad * 2)
	{
		ball.pos.x = 1;
	}
	if (ball.pos.y > world_size.y - ball.rad * 2)
	{
		ball.pos.y = 1;
	}
}

void VeganActor::update(float delta_time)
{
	std::lock_guard<std::mutex> lck(*mutex);

	if(selected)
	{
		std::cout << "ID: " << id << std::endl;
		std::cout << "Energy: " << energy << std::endl;
		std::cout << "Size: " << size << std::endl;
		std::cout << "Child Starting Energy: " << child_starting_energy << std::endl;
		std::cout << "Child Energy Threshold: " << child_energy_threshold << std::endl;
	}
	int neighbors = 0;
	bool target = false;
	for (const auto& a : sighted_actors)
	{
		if (a != nullptr) {
			neighbors++;
			if (PlantActor* pa = dynamic_cast<PlantActor*>(a.get()))
			{
				target = true;
				auto vec = pa->ball.pos - ball.pos;

				desired_vector = vec;
				if (Ball::magsq(vec) < (ball.rad + pa->ball.rad) * (ball.rad + pa->ball.rad) * 2 && energy < size)
				{
					pa->ball.vel = ball.vel;
					pa->projected_growth -= (size/600) * delta_time;
					energy += (size / 600) * delta_time * 0.2f;
					PlantActor::world_nutrition = PlantActor::world_nutrition + (size / 600) * delta_time * 0.8f;
					desired_vector = ball.pos;
				}
				// do Child specific stuff
			}
			
		}
		//std::cout << sqrt(Ball::magsq(avoid)) << std::endl;
	}
	const float spent_energy = (((size / 600) * (size / 600) * (size / 600)) + (Ball::magsq(ball.vel)/(25 * 25)) + sight_range/250) * 0.002 * delta_time;
	PlantActor::world_nutrition = PlantActor::world_nutrition + spent_energy;
	energy -= spent_energy;
	if(energy < size/10.f && !for_deletion)
	{
		for_deletion = true;
		PlantActor::world_nutrition = PlantActor::world_nutrition + energy;
	} else if (energy > child_energy_threshold * size && energy > child_starting_energy)
	{
		std::shared_ptr<VeganActor> temp = std::make_shared<VeganActor>(VeganActor(*this));
		temp->energy = child_starting_energy;
		
		for_creation.push_back(temp);
		energy -= child_starting_energy;
	}

	const auto walls = avoid_walls();
	if (Ball::magsq(walls) > 0)
	{
		if (turn_countdown <= 0)
		{
			turn_countdown = rand() % 750;
			auto ang = atan2f(walls.y, walls.x);
			wander_angle = ang + (rand() % 200 - 100) / 1000.f;
		}
	}
	
	if(!target)
	{
		if (turn_countdown <= 0)
		{
			turn_countdown = rand() % 750;
			wander_angle = ((rand() % 50 - 25) / 1000.f) * 6.28318530718f;
		}
		desired_vector = { cosf(facing + wander_angle), sinf(facing + wander_angle) };
		desired_vector *= 1000.f;
		turn_countdown--;
	}else
	{
		turn_countdown = 0;
	}
	
	auto steer = desired_vector - ball.vel + walls * 5.f;
	if(Ball::magsq(steer) > max_turn * max_turn)
	{
		steer /= sqrtf(Ball::magsq(steer));
		steer *= max_turn;
	}
	ball.acc = steer;
	facing = atan2f(ball.vel.y, ball.vel.x);

	if (isnan((float)PlantActor::world_nutrition))
	{
		std::cout << "Nanned";
	}
}
