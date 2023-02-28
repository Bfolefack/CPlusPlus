#pragma once
#include <unordered_map>
#include <iostream>

template <typename L, typename  R> class BiMap
{
public:
	std::unordered_map<L, R> L_map;
	std::unordered_map<R, L> R_map;
	//BiMap<L, R>();
	void insert(L left, R right);
	void erase_left(L left);
	void erase_right(R right);
	void print();
	int size();
	auto find_left(L left);
	auto find_right(R right);
	auto begin_left();
	auto end_left();
	auto begin_right();
	auto end_right();
};


template <typename  L, typename  R>
void BiMap<L, R>::insert(L key, R value)
{

	L_map.emplace(key, value);
	R_map.emplace(value, key);
}

template <typename L, typename R>
void BiMap<L, R>::erase_left(L left)
{
	auto temp = L_map[left];
	L_map.erase(left);
	R_map.erase(temp);
}

template <typename L, typename R>
void BiMap<L, R>::erase_right(R right)
{
	auto temp = R_map[right];
	R_map.erase(right);
	L_map.erase(temp);
}

template <typename L, typename R>
void BiMap<L, R>::print()
{
	for (auto i : L_map)
	{
		std::cout << i.first << ", " << i.second << std::endl;
	}
}


template <typename L, typename R>
int BiMap<L, R>::size()
{
	if (L_map.size() == R_map.size())
		return L_map.size();
	return -1;
}

template <typename L, typename R>
auto BiMap<L, R>::find_left(L key)
{
	return L_map.find(key);
}

template <typename L, typename R>
auto BiMap<L, R>::find_right(R right)
{
	return R_map.find(right);
}

template <typename L, typename R>
auto BiMap<L, R>::begin_left()
{
	return L_map.begin();
}

template <typename L, typename R>
auto BiMap<L, R>::end_left()
{
	return L_map.end();
}

template <typename L, typename R>
auto BiMap<L, R>::begin_right()
{
	return R_map.begin();
}

template <typename L, typename R>
auto BiMap<L, R>::end_right()
{
	return R_map.end();
}
