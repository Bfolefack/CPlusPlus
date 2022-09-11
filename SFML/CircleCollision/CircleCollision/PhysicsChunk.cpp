#include "PhysicsChunk.h"
#include "Ball.h"
#include "Collision.h"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <vector>
#include <deque>
#include <unordered_map>

using std::vector;
using std::queue;
using std::shared_ptr;
using std::unordered_map;

PhysicsChunk::PhysicsChunk()
{
	size = sf::Vector2f(1900, 1000);
	balls = unordered_map<int, shared_ptr<Ball>>();
	exiting_balls = std::list<int>();
	collision_stack = queue<Collision>();
	selectedBall = nullptr;
	velSelectedBall = nullptr;
	mousePos = sf::Vector2f(0, 0);
}

PhysicsChunk::PhysicsChunk(sf::Vector2f pos, sf::Vector2f size)
{
}

std::list<int> PhysicsChunk::update(float deltaTime)
{
	exiting_balls = std::list<int>();

	for (auto const b : balls) {
		(*b.second).update();
		if (b.second->pos.x < pos.x || b.second->pos.x > pos.x + size.x || b.second->pos.y < pos.y || b.second->pos.y > pos.y + size.y)
		{
			exiting_balls.push_back(b.first);
		}
	}

	for (auto const i : exiting_balls)
	{
		remove_ball(i);
	}

	if (selectedBall != nullptr) {
		selectedBall->pos = mousePos;
	}
	/*for (auto b1 : balls) {
		if (b1.second->pos.x + b1.second->rad  > width) {
			b1.second->vel.x *= -1 * b1.second->elasticity;
			b1.second->pos.x = width - b1.second->rad;
			b1.second->collision = true;
		}
		if (b1.second->pos.x - b1.second->rad < 0) {
			b1.second->vel.x *= -1 * b1.second->elasticity;
			b1.second->pos.x = b1.second->rad;
			b1.second->collision = true;
		}
		if (b1.second->pos.y + b1.second->rad > height) {
			b1.second->vel.y *= -1 * b1.second->elasticity;
			b1.second->pos.y = height - b1.second->rad;
			b1.second->collision = true;
		}
		if (b1.second->pos.y - b1.second->rad < 0) {
			b1.second->vel.y *= -1 * b1.second->elasticity;
			b1.second->pos.y = b1.second->rad;
			b1.second->collision = true;
		}
	}*/
	std::deque<shared_ptr<Ball>> temp_balls;
	
	for (auto b : balls) {
		temp_balls.push_back(b.second);
	}
	collision_stack = queue<Collision>();
	for (const auto b1 : balls) {
		temp_balls.pop_front();
		for (const auto b2 : temp_balls) {
			auto coll = Ball::collide(b1.second, b2);
			if (coll.collision)
				collision_stack.push(coll);
		}
	}

	resolve_collisions(deltaTime);
	return exiting_balls;
}

void PhysicsChunk::resolve_collisions(float delta_time) {
	const auto temp = collision_stack;
	while (!collision_stack.empty()) {

		auto collision = collision_stack.front();
			collision_stack.pop();
		const auto b1 = balls[collision.ball1];
		const auto b2 = balls[collision.ball2];

		//Complex Maths
		const auto normal = -(b2->pos - b1->pos) / sqrt(Ball::magsq((b2->pos - b1->pos)));
		const auto tangent = sf::Vector2f(-normal.y, normal.x);


		const auto tan_dp1 = (b1->vel.x) * tangent.x + (b1->vel.y) * tangent.y;
		const auto tan_dp2 = (b2->vel.x) * tangent.x + (b2->vel.y) * tangent.y;

		const auto norm_dp1 = (b1->vel.x) * normal.x + (b1->vel.y) * normal.y;
		const auto norm_dp2 = (b2->vel.x) * normal.x + (b2->vel.y) * normal.y;

		////Conservation of momentum
		const auto m1 = (norm_dp1 * (b1->mass - b2->mass) + 2.f * b2->mass * norm_dp2) / (b1->mass + b2->mass);
		const auto m2 = (norm_dp2 * (b2->mass - b1->mass) + 2.f * b1->mass * norm_dp1) / (b1->mass + b2->mass);
		b1->delta_time = (delta_time < 1 ? delta_time : 1);
		b2->delta_time = (delta_time < 1 ? delta_time : 1);
		b1->collisionStack.insert({ b2->id, {collision.ball1_overlap, (((tangent * tan_dp1) + (normal * m1)) * (b1->elasticity * b2->elasticity))} });
		b2->collisionStack.insert({ b1->id, {collision.ball2_overlap, (((tangent * tan_dp2) + (normal * m2)) * (b1->elasticity * b2->elasticity))} });
	}
	collision_stack = temp;
}

void drawLine(sf::Vector2f point1, sf::Vector2f point2, sf::Color color, sf::RenderWindow& window) {
	sf::Vertex line[2];
	line[0].position = point1;
	line[1].position = point2;
	line[0].color = color;
	line[1].color = color;
	window.draw(line, 2, sf::Lines);
}

void PhysicsChunk::draw( sf::RenderWindow& window)
{
	mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
	for (auto b : balls) {
		b.second->draw(window);
	}
	if (velSelectedBall != nullptr) {
		drawLine(velSelectedBall->pos, mousePos, sf::Color::Blue, window);
	}
	for (int i = 0; i < collision_stack.size(); i++) {
		Collision temp = collision_stack.front();
		collision_stack.pop();
		drawLine(balls[temp.ball1]->pos, balls[temp.ball2]->pos, sf::Color::Red, window);
	}
}



void PhysicsChunk::select_ball()
{
	for (auto b : balls) {
		if (b.second->isInside(mousePos)) {
			remove_ball(b.first);
			//selectedBall = b.second;
			return;
		}
	}
}

void PhysicsChunk::deselect_ball()
{
	selectedBall = nullptr;
}

void PhysicsChunk::vel_select_ball()
{
	for (auto b : balls) {
		if (b.second->isInside(mousePos)) {
			velSelectedBall = b.second;
			return;
		}
	}
}

void PhysicsChunk::vel_deselect_ball()
{
	if (velSelectedBall != nullptr) {
		velSelectedBall->vel = ((velSelectedBall->pos - mousePos));
		velSelectedBall = nullptr;
	}
}



void PhysicsChunk::add_ball(int id, const Ball& ball)
{
	balls.insert({id, std::make_shared<Ball>(ball) });
}

void PhysicsChunk::add_ball(int id, const std::shared_ptr<Ball>& ball)
{
	balls.insert({ id, ball });
}

void PhysicsChunk::remove_ball(int id)
{
	balls.erase(id);
}


