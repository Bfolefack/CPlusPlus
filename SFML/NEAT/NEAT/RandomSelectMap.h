#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>


template <typename K, typename  V> class RandomSelectMap
{
public:
	std::vector<K> vec;
	std::unordered_map<K, V> map;

	//RandomSelectMap<K, V>();
	void insert(K key, V value);
	void erase(K key);
	void print();
	std::pair<K,V> get_random();
	V& operator[](K key);
	int size();
	auto find(K key);
	auto begin();
	auto end();
};


template <typename  K, typename  V>
void RandomSelectMap<K, V>::insert(K key, V value)
{
	
	auto out = map.emplace(key, value).second;
	if (out) {
		vec.push_back(key);
		return;
	}
	auto index = std::find(vec.begin(), vec.end(), key);
	vec.erase(index);
	map.erase(key);
	insert(key, value);
}

template <typename K, typename V>
void RandomSelectMap<K, V>::erase(K key)
{
	auto index = find(vec.begin(), vec.end(), key);
	if (index != vec.end()) {
		vec.erase(index);
		map.erase(key);
	}
}

template <typename K, typename V>
void RandomSelectMap<K, V>::print()
{
	for (auto i : map)
	{
		std::cout << i.first << ", " << i.second << std::endl;
	}
}

template <typename K, typename V>
std::pair<K, V> RandomSelectMap<K, V>::get_random()
{
	auto ind = rand() % vec.size();
	auto key = vec[ind];
	auto value = map[key];
	return {key, value};
}

template <typename K, typename V>
V& RandomSelectMap<K, V>::operator[](K key)
{
	if(map.count(key) > 0){
		return map[key];
	}
}

template <typename K, typename V>
int RandomSelectMap<K, V>::size()
{
	if (map.size() == vec.size())
		return map.size();
	std::cout << "SIZE MISMATCH RANDOMSELECTMAP" << std::endl;
	return -1;
}

template <typename K, typename V>
auto RandomSelectMap<K, V>::find(K key)
{
	return map.find(key);
}

template <typename K, typename V>
auto RandomSelectMap<K, V>::begin()
{
	return map.begin();
}

template <typename K, typename V>
auto RandomSelectMap<K, V>::end()
{
	return map.end();
}
