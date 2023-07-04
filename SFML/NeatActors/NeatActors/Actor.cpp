#include "Actor.h"

#include "NeatConfig.h"

Actor::Actor() : NeatCore(), PhysicsCore() {
	holder = -1;
	holder_str = 0;
	holding = nullptr;
	brainless = true;
	delta_time = 1;
}

Actor::Actor(Ball& b) : PhysicsCore(b) {
	holder = -1;
	holder_str = 0;
	holding = nullptr;
	brainless = true;
	delta_time = 1;
}

Actor::Actor(Ball& b, int s) : PhysicsCore(b) {
	holder = -1;
	holder_str = 0;
	holding = nullptr;
	brainless = true;
	delta_time = 1;
}

Actor::Actor(const Genome& g) : NeatCore(g) {
	holder = -1;
	holder_str = 0;
	holding = nullptr;
	brainless = false;
	delta_time = 1;
}

Actor::Actor(const Ball& b, const Genome& g) : NeatCore(g), PhysicsCore(b) {
	holder = -1;
	holder_str = 0;
	holding = nullptr;
	brainless = false;
	delta_time = 1;
}

void Actor::set_inputs(std::vector<float> inputs)
{
}

void Actor::act()
{
}

Genome Actor::breed(NeatCore* other)
{
	Genome new_genome = Genome::crossover(genome, other->genome);
	new_genome.mutate();
	return new_genome;
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

int Actor::get_network_size()
{
	return genome.connections.map.size();
}

std::tuple<float, float, float> Actor::get_mismatch(std::shared_ptr<NeatCore> interloper)
{
	return NeatCore::get_mismatch(interloper);
}

void Actor::update(float delta_time)
{
}

void Actor::NEAT_update()
{
}

bool Actor::hit(sf::Vector2f source, float angle, float range, sf::Vector2f point, float rad) const
{
	const auto x1 = source.x - point.x;
	const auto y1 = source.y - point.y;
	const float x2 = x1 + range * cos(angle);
	const float y2 = y1 + range * sin(angle);
	const auto dr_sq = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	const auto D = x1 * y2 - x2 * y1;
	const float hit = rad * rad * dr_sq - D * D;
	return hit >= 0;
	//auto dr_sq = 
}

std::pair<int, float> Actor::hit(sf::Vector2f source, float angle, float range,
                                 const std::unordered_map<int, std::shared_ptr<Actor>>& actors) const
{
	float dist = range * range;
	int out = -1;
	for (const auto& a : actors)
	{

		if (a.first == id)
			continue;
		if (hit(source, angle, range, a.second->ball.pos, a.second->ball.rad))
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
	return { out, dist };
}

void Actor::hold(std::shared_ptr<Actor> a)
{
	if (a != nullptr) {
		std::lock_guard<std::mutex> lck(*(a->mutex));
		
		if (holding == nullptr) {
			if (a->holder != id && a->holder != -1)
			{
				return;
			}
			holding = std::make_shared<Actor>(*a);
		} else if (a->id != holding->id) {
			holding->holder = -1;
			holding->holder_str = 0;
		}
	}
	if (holding != nullptr) {
		std::lock_guard<std::mutex> lck(*(holding->mutex));
		auto d = sqrt(Ball::magsq(ball.pos - holding->ball.pos));
		holding->ball.pos = ball.pos + sf::Vector2f{ cos(facing) * d, sin(facing) * d };
	}
}

void Actor::hold(std::shared_ptr<Actor> a, float strength)
{
	if (a != nullptr) {
		std::lock_guard<std::mutex> lck(*(a->mutex));

		if (holding == nullptr) {
			if (a->holder != id && a->holder != -1)
			{
				return;
			}
			holding = std::make_shared<Actor>(*a);
		}
		else if (a->id != holding->id) {
			holding->holder = -1;
			holding->holder_str = 0;
		}
	}
	if (holding != nullptr) {
		std::lock_guard<std::mutex> lck(*(holding->mutex));
		auto d = sqrt(Ball::magsq(ball.pos - holding->ball.pos));
		holding->ball.pos = ball.pos + sf::Vector2f{ cos(facing) * d, sin(facing) * d };
	}
}

void Actor::ray_cast(const std::unordered_map<int, std::shared_ptr<Actor>>& actors)
{
	//std::lock_guard<std::mutex> lck(*mutex);
	int count = 0;
	for (const auto a : ray_angles)
	{
		const auto ind = hit(ball.pos, a + facing, sight_range, actors);
		if (ind.first == -1)
		{
			sighted_actors[count] = nullptr;
			sq_sighted_actor_dists[count] = -1.f;

		}
		else
		{
			sighted_actors[count] = actors.at(ind.first);
			sq_sighted_actor_dists[count] = ind.second;
		}
		count++;
	}
	//if(actors.size() > 30)
	//{
	//	std::cout << "ray_cast: " << actors.size() << std::endl;
	//}
}
