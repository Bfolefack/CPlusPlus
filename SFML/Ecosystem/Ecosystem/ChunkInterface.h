#pragma once


#include <list>
#include <memory>
#include <unordered_set>

class Actor;
class Ball;
class PhysicsChunk;
using std::shared_ptr;

struct ChunkInterface
{
public:
	//std::list<shared_ptr<Ball>> limbo_list;
	std::unordered_set<int> exiting_actors;
	std::unordered_set<int> for_deletion;
	std::unordered_set<shared_ptr<Actor>> for_creation;

	//ChunkInterface();
	//ChunkInterface(PhysicsChunk& c, int id, std::shared_ptr<std::mutex> m);
	ChunkInterface();
};


