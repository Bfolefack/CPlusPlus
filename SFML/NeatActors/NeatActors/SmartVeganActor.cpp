#include "SmartVeganActor.h"

#include "NeatConfig.h"
#include "PlantActor.h"


void SmartVeganActor::hold(std::shared_ptr<Actor> a, float strength)
{
	if (a == nullptr)
		return;
	if (const auto pa = dynamic_cast<PlantActor*>(a.get())) {

		//std::lock_guard<std::mutex> lck(*(a->mutex));

		if (holding == nullptr) {
			//If not holding a plant
			if (pa->holder != id && strength * size > pa->holder_str)
			{
				pa->holder = id;
				pa->holder_str = strength * size;
				holding = a;
			}
		}
		else if (pa->id != holding->id) {
			//If holding a plant, but not the one we're trying to hold

			holding->holder = -1;
			holding->holder_str = 0;
			holding = nullptr;
		}else if (holding != nullptr) {
			//If holding a plant, and it's the one we're trying to hold

			//std::lock_guard<std::mutex> lck(*(holding->mutex));
			auto d = sqrt(Ball::magsq(ball.pos - holding->ball.pos));
			//holding->ball.pos = ball.pos + sf::Vector2f{ cos(facing) * d, sin(facing) * d };
			holding->ball.vel = ball.vel;
			float bite_size = (size / 60) * strength * delta_time;
			float energy_gain = bite_size > pa->size + pa->projected_growth ? pa->size + pa->projected_growth : bite_size;
			energy += energy_gain;
			std::cout << "energy gain: " << energy_gain << "\n" << std::endl;
			lifetime_energy_gain += energy_gain;
			pa->projected_growth -= energy_gain;
		}
	}
}

void SmartVeganActor::bite(float delta_time, std::shared_ptr<Actor> closest_actor, float sq_closest_actor_dist, std::vector<float> outputs)
{
	if (outputs[2] < 0)
		return;
	energy -= (size / 600) * outputs[2] * delta_time * 0.1f;
	bite_cooldown = agility * outputs[2];
	if (closest_actor == nullptr)
		return;
	if (sq_closest_actor_dist < ball.rad * ball.rad * 4 + closest_actor->ball.rad * closest_actor->ball.rad)
		hold(closest_actor, outputs[2]);
}

void SmartVeganActor::NEAT_update()
{
	std::vector<float> inputs;
	// network inputs [energy, ray1_dist, ray1_size, ray1_red, ray1_green, ray1_blue, ray2_dist, ray2_size, ray2_red, ray2_green, ray2_blue, ray3_dist, ray3_size, ray3_red, ray3_green, ray3_blue, ray4_dist, ray4_size, ray4_red, ray4_green, ray4_blue, ray5_dist, ray5_size, ray5_red, ray5_green, ray5_blue]
	inputs.push_back(energy / child_starting_energy);
	closest_actor = nullptr;
	sq_closest_actor_dist = 10000000000.f;
	if (sighted_actors.size() == 5) {
		for (int i = 0; i < 5; i++)
		{
			if (sq_sighted_actor_dists[i] < sq_closest_actor_dist)
			{
				sq_closest_actor_dist = sq_sighted_actor_dists[i];
				closest_actor = sighted_actors[i];
			}
			if (sighted_actors[i] == nullptr)
			{
				inputs.push_back(-1);
				inputs.push_back(-1);
				inputs.push_back(-1);
				inputs.push_back(-1);
				inputs.push_back(-1);
			}
			else
			{
				inputs.push_back(sqrt(sq_sighted_actor_dists[i]) / sight_range);
				inputs.push_back(sighted_actors[i]->ball.rad / ball.rad);
				inputs.push_back(sighted_actors[i]->sprite_color.r / 255.0f);
				inputs.push_back(sighted_actors[i]->sprite_color.g / 255.0f);
				inputs.push_back(sighted_actors[i]->sprite_color.b / 255.0f);
			}
		}
	}
	// outputs [move_speed, turn, bite]
	outputs = genome.feed_forward(inputs);
}

void SmartVeganActor::update(float delta_time)
{
	std::lock_guard<std::mutex> lck(*mutex);

	this->delta_time = delta_time;

	if (selected)
	{
		std::cout << "ID: " << id << std::endl;
		std::cout << "Energy: " << energy << std::endl;
		std::cout << "Size: " << size << std::endl;
		std::cout << "Child Starting Energy: " << child_starting_energy << std::endl;
		std::cout << "Bite Cooldown:" << bite_cooldown << std::endl;
		std::cout << "Lifetime Energy Gain: " << lifetime_energy_gain << std::endl;
	}
	if (outputs.size() == NeatConfig::max_outputs) {
		

		auto walls = avoid_walls();
		auto wall_angle = atan(-walls.y / walls.x);
		if (walls.y == 0 && walls.x == 0)
			facing += (outputs[1]) * agility / 100;
		else
			if (facing - wall_angle > agility) {
				facing += (facing - wall_angle) * agility;
			}
			else
			{
				facing = wall_angle;
			}


		ball.acc = sf::Vector2f(cos(facing), sin(facing)) * (outputs[0]) * max_speed * agility;
		auto vel_component = (Ball::magsq(ball.acc));
		auto mass_component = (size / 100.f * size / 100.f * size / 100.f);
		energy -= (vel_component + mass_component) * 0.0025f * delta_time;
		bite_cooldown -= delta_time;
		bite(delta_time, closest_actor, sq_closest_actor_dist, outputs);
	}
}

void SmartVeganActor::mutate()
{
	max_speed = mutate(max_speed);
	if (max_speed < 1)
		max_speed = 1;
	agility = mutate(agility);
	if (agility < 0.001f)
		agility = 0.001f;
	size = mutate(size);
	if (size < 5)
		size = 5;
	sight_range = mutate(sight_range);
	if (sight_range < 10)
		sight_range = 10;
	child_starting_energy = mutate(child_starting_energy);
	mutation_rate = mutate(mutation_rate);
	fov = mutate(fov);
	Red = mutate(Red);
	Green = mutate(Green);
	Blue = mutate(Blue);
	ball.rad = size;
	ball.max_vel = max_speed;
	ball.sqrt_max_vel = sqrt(max_speed);
}

float SmartVeganActor::mutate(float num) const
{
	auto mutagen = (1 + (rand() % 1000 - 500) / (1000.f / mutation_rate));
	return num * mutagen;
}

float SmartVeganActor::rand_range(int min, int max)
{
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

void SmartVeganActor::ray_cast(const std::unordered_map<int, std::shared_ptr<Actor>>& actors)
{
	Actor::ray_cast(actors);
	NEAT_update();
}

void SmartVeganActor::initialize_sight_rays()
{
	for (float f = -3.14159265358979323846f * fov; f < 3.14159265358979323846f * fov; f += 2 * 3.14159265358979323846f * fov / 5)
	{
		ray_angles.push_back(f);
		sighted_actors.push_back(nullptr);
		sq_sighted_actor_dists.push_back(100000000.f);
	}
}

SmartVeganActor::SmartVeganActor(const Ball& b)
{
	ball = b;
	ball.active = true;
	ball.always_active = true;
	//ball.friction = 0.025f;
	

	facing = ((rand() % 6284) - 3142) / 1000.f;
	ball.vel = { cos(facing), sin(facing) };
	ball.vel *= 0.05f;
	ball.friction = 0.01f;

	max_speed = rand_range(10, 50);
	agility = rand_range(10, 500) / 10000.f;
	sight_range = rand_range(100, 1000);
	size = rand_range(20, 100);
	energy = size;
	child_starting_energy = 100;
	mutation_rate = rand_range(10, 100) / 200.f;
	fov = rand_range(0, 1);
	Red = rand_range(0, 255);
	Green = rand_range(0, 255);
	Blue = rand_range(0, 255);

	mutate();
	bite_cooldown = 0;
	sprite_id = 0;
	lifetime_energy_gain = 0;
	sprite_color = sf::Color(Red, Green, Blue);

	initialize_sight_rays();

	epoch_fitness = 0;
	batch_fitness = 0;
	alive = true;
	genome = Genome(0);
}

SmartVeganActor::SmartVeganActor(const SmartVeganActor& parent) : Actor(parent.ball, parent.genome)
{
	ball = parent.ball;
	ball.active = true;
	ball.always_active = true;
	//ball.friction = 0.025f;
	

	facing = ((rand() % 6284) - 3142) / 1000.f;
	ball.vel = { cos(facing), sin(facing) };
	ball.vel *= 0.05f;
	ball.friction = 0.01f;



	sprite_id = parent.sprite_id;

	agility = parent.agility;
	max_speed = parent.max_speed;
	mutation_rate = parent.mutation_rate;
	sight_range = parent.sight_range;
	size = parent.size;
	energy = parent.child_starting_energy;
	child_starting_energy = parent.child_starting_energy;
	fov = parent.fov;
	Red = parent.Red;
	Blue = parent.Blue;
	Green = parent.Green;
	mutate();
	bite_cooldown = 0;
	sprite_id = 0;
	lifetime_energy_gain = 0;
	sprite_color = sf::Color(Red, Green, Blue);

	initialize_sight_rays();
}
