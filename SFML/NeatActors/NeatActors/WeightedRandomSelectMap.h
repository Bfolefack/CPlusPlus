#pragma once
#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <map>
#include "BiMap.h"

template <typename K, typename  V> class WeightedRandomSelectMap
{
public:
	std::vector<K> vec;
	std::unordered_map<K, V> map;
	BiMap<double, K> weights;
	std::set<double> weight_set;
	double total_weight;

	void place(K key, V value, double weight);
	void remove(K key);
	//WeightedRandomSelectMap<K, V>();
	void insert(K key, V value, double weight);
	void erase(K key);
	void print();
	std::pair<K, V> get_random();
	V& operator[](K key);
	int size();
	auto find(K key);
	auto begin();
	auto end();
	void sum_weights();
	auto get_best();
	auto get_worst();
	auto pop_worst();
	auto pop_worst(int pop);
	void add_all(typename std::unordered_map<K, V>::iterator pair, typename std::unordered_map<K, V>::iterator end);
};


template <typename  K, typename  V>
void WeightedRandomSelectMap<K, V>::place(K key, V value, double weight)
{

	auto out = map.emplace(key, value).second;
	if (out) {
		vec.push_back(key);
		weights.insert(weight + (rand() % 1000) / 1000000.f, key);
		return;
	}
	auto index = std::find(vec.begin(), vec.end(), key);
	vec.erase(index);
	map.erase(key);
	weights.erase_right(key);
	place(key, value, weight);
}

template <typename K, typename V>
void WeightedRandomSelectMap<K, V>::remove(K key)
{
	auto index = std::find(vec.begin(), vec.end(), key);

	if (index != vec.end()) {
		vec.erase(index);
		map.erase(key);
		weights.erase_right(key);
	}
}

template <typename  K, typename  V>
void WeightedRandomSelectMap<K, V>::insert(K key, V value, double weight)
{

	place(key, value, weight);
	sum_weights();
}

template <typename K, typename V>
void WeightedRandomSelectMap<K, V>::erase(K key)
{

	remove(key);
	
	sum_weights();
}

template <typename K, typename V>
void WeightedRandomSelectMap<K, V>::print()
{
	for (auto i : map)
	{
		std::cout << i.first << ", " << i.second << std::endl;
	}
}

template <typename K, typename V>
std::pair<K, V> WeightedRandomSelectMap<K, V>::get_random()
{
	const auto num = ((rand() - 1)/static_cast<double>(RAND_MAX)) * total_weight;
	auto sum = 0.f;
	for (auto i : weights.L_map)
	{
		sum += i.first;
		if (sum > num) {
			return std::make_pair(i.second, map[i.second]);
		}
	}
}

template <typename K, typename V>
V& WeightedRandomSelectMap<K, V>::operator[](K key)
{
	if (map.count(key) > 0) {
		return map[key];
	}
	return {};
}

template <typename K, typename V>
int WeightedRandomSelectMap<K, V>::size()
{
	if (map.size() == vec.size())
		return map.size();
	std::cout << "SIZE MISMATCH WeightedRandomSelectMap" << std::endl;
	return -1;
}

template <typename K, typename V>
auto WeightedRandomSelectMap<K, V>::find(K key)
{
	return map.find(key);
}

template <typename K, typename V>
auto WeightedRandomSelectMap<K, V>::begin()
{
	return map.begin();
}

template <typename K, typename V>
auto WeightedRandomSelectMap<K, V>::end()
{
	return map.end();
}

template <typename K, typename V>
void WeightedRandomSelectMap<K, V>::sum_weights()
{
	weight_set.clear();
	total_weight = 0;
	for (auto i : weights.L_map)
	{
		total_weight += i.first;
		weight_set.insert(i.first);
	}
}

template <typename K, typename V>
auto WeightedRandomSelectMap<K, V>::get_best()
{
	auto greatest = *(--weight_set.end());
	K best = weights.find_left(greatest)->second;
	return map.find(best);
}


template <typename K, typename V>
auto WeightedRandomSelectMap<K, V>::get_worst()
{
	auto greatest = *(weight_set.begin());
	K best = weights.find_left(greatest)->second;
	return map.find(best);
}

template <typename K, typename V>
auto WeightedRandomSelectMap<K, V>::pop_worst()
{
	auto least = *(weight_set.begin());
	K worst = weights.find_left(least)->second;
	erase(worst);
}

template <typename K, typename V>
auto WeightedRandomSelectMap<K, V>::pop_worst(int pop)
{

	auto least = weight_set.begin();
	for (int i = 0; i < pop; i++) {
		K worst = weights.find_left(*least)->second;
		++least;
		remove(worst);
	}
	sum_weights();
}

template <typename K, typename V>
void WeightedRandomSelectMap<K, V>::add_all(typename std::unordered_map<K, V>::iterator pair, typename std::unordered_map<K, V>::iterator end)
{
	do
	{
		K key = pair->first;
		V value = pair->second;
		double weight = pair->second->batch_fitness;
		place(key, value, weight);
	} while (++pair != end);
	sum_weights();
}
