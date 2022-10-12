#pragma once


#include <list>
#include <memory>
#include <unordered_set>

class Ball;
class PhysicsChunk;
using std::shared_ptr;

struct ChunkInterface
{
public:
	//std::list<shared_ptr<Ball>> limbo_list;
	std::unordered_set<int> exiting_balls;

	//ChunkInterface();
	//ChunkInterface(PhysicsChunk& c, int id, std::shared_ptr<std::mutex> m);
	ChunkInterface();
};


