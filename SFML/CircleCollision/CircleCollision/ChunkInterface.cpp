#include "ChunkInterface.h"
#include "PhysicsChunk.h"



ChunkInterface::ChunkInterface()
{
	exiting_balls = std::unordered_set<int>();
	limbo_list = std::list<shared_ptr<Ball>>();
	//thread = move(thr);
}
