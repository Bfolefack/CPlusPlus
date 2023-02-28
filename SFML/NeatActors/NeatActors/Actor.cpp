#include "Actor.h"

Actor::Actor() : NeatActor(), PhysicsActor() {}

Actor::Actor(Ball& b) : PhysicsActor(b) {}

Actor::Actor(Ball& b, int s) : PhysicsActor(b) {}

Actor::Actor(const Genome& g) : NeatActor(g) {}

Actor::Actor(const Ball& b, const Genome& g) : NeatActor(g), PhysicsActor(b) {}

void Actor::set_inputs(std::vector<float> inputs)
{
}

void Actor::act()
{
}

Actor* Actor::breed(NeatActor* other)
{
	Genome new_genome = Genome::crossover(genome, other->genome);
	new_genome.mutate();
	return new Actor{ new_genome };
}

double Actor::calculate_fitness()
{
	return 0;
}

void Actor::normalize_fitness()
{
}

void Actor::epoch()
{
	batch_fitness += epoch_fitness;
	epoch_fitness = 0;
}

Actor* Actor::clone()
{
	return new Actor{ genome };
}

int Actor::size()
{
	return genome.connections.map.size();
}

void Actor::update(float delta_time)
{
}

int Actor::hit(sf::Vector2f source, float angle, float range, const std::unordered_map<int, std::shared_ptr<Actor>>& actors) const
{
	float dist = range * range;
	int out = -1;
	for (const auto& a : actors)
	{

		if (a.first == id)
			continue;
		if (PhysicsActor::hit(source, angle, range, a.second->ball.pos, a.second->ball.rad))
		{
			const auto d = (source.x - a.second->ball.pos.x) * (source.x - a.second->ball.pos.x) + (source.y - a.second->ball.pos.y) * (source.y - a.second->ball.pos.y);
			if (d < dist)
			{
				out = a.first;
				dist = d;
			}
		}
		//casted.insert(a.first);
	}
	return out;
}

void Actor::ray_cast(const std::unordered_map<int, std::shared_ptr<Actor>>& actors)
{
	//std::lock_guard<std::mutex> lck(*mutex);
	int count = 0;
	for (const auto a : ray_angles)
	{
		const auto ind = hit(ball.pos, a + facing, sight_range, actors);
		if (ind == -1)
		{
			sighted_actors[count] = nullptr;
		}
		else
		{
			sighted_actors[count] = actors.at(ind);
		}
		count++;
	}
	//if(actors.size() > 30)
	//{
	//	std::cout << "ray_cast: " << actors.size() << std::endl;
	//}
}
