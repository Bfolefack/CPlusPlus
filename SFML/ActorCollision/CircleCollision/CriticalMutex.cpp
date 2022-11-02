#include "CriticalMutex.h"
#include "ChunkInterface.h"
#include <unordered_map>
#include <memory>

std::atomic<bool> CriticalMutex::all_frozen = true;

CriticalMutex::CriticalMutex()
{
	chunk_interface = std::unordered_map<int, std::unique_ptr<ChunkInterface>>();
	mutex = std::make_unique<std::mutex>();
}