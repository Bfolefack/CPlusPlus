#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>


class Actor;
struct ChunkInterface;

struct CriticalMutex
{
	std::thread thread;
	std::unique_ptr<std::mutex> mutex;

	std::unordered_map<int, std::unique_ptr<ChunkInterface>> chunk_interface;
	std::list<int> chunk_ids;

	std::unordered_map<int, std::list<std::shared_ptr<Actor>>> limbo_list;
	std::unordered_set<int> exiting_actors;
	std::unordered_set<int> for_deletion;
	std::vector<std::shared_ptr<Actor>> for_creation;

	static std::atomic<bool> all_frozen;

	CriticalMutex();
};

