#include "Ball.h"
#include "Collision.h"

#include <cmath>
#include <string>
#include <unordered_map>
#include <iostream>

#include "PhysicsManager.h"

using std::shared_ptr;


Ball::Ball()
{
	id = -1;
	rad = -1;
	pos = sf::Vector2f();
	vel = sf::Vector2f();
	acc = sf::Vector2f();
	active = false;
	mass = 1;
	elasticity = 1;
	friction = 0;
	always_active = false;
	collisionStack = std::unordered_map<int, std::tuple<float, sf::Vector2f, sf::Vector2f>>();
	max_vel = global_max_vel;
	sqrt_max_vel = global_sqrt_max_vel;
}


Ball::Ball(float r, float x, float y, float m, float e, float f) {
	id = -1;
	rad = r;
	pos = sf::Vector2f(x, y);
	vel = sf::Vector2f((((rand() % 2) - 0.5f) * 1.f), (((rand() % 2 - 0.5f)) * 1.f));
	//acc = sf::Vector2f(((rand() % 2 - 0.5f) * 0.5f), ((rand() % 2 - 0.5f) * 0.5f));
	//vel = sf::Vector2f(vx, vy);
	acc = sf::Vector2f(0, 0);
	active = true;
	always_active = false;
	mass = r * r * 3.14156f;
	elasticity = e;
	friction = f;
	collisionStack = std::unordered_map<int, std::tuple<float, sf::Vector2f, sf::Vector2f>>();
	max_vel = global_max_vel;
	sqrt_max_vel = global_sqrt_max_vel;
}

void Ball::update(const float& delta_time)
{
	//if (!collision)
	//acc = sf::Vector2f(0, 0);
	//std::unique_lock<std::mutex> lock(*mutex);
	//auto start = std::chrono::high_resolution_clock::now();
	//if (lock.try_lock()) {
	if(id == PhysicsManager::global_selected_actor)
	{
		std::cout << "Ball " << id << " is selected" << std::endl;
		PhysicsManager::global_selected_actor = -1;
	}
	if (!collisionStack.empty()) {
		
		vel = sf::Vector2f(0, 0);
		//auto ogVel = vel;
		auto tempPos = sf::Vector2f(0, 0);
		float smallestDot = 50;

		float delta_time_sum = 0;

		for (std::pair<const int, std::tuple<float, sf::Vector2<float>, sf::Vector2<float>>> i : collisionStack) {
			sf::Vector2f overlap = std::get<1>(i.second);
			sf::Vector2f v = std::get<2>(i.second);
			const auto dot = overlap.x * acc.x + overlap.y * acc.y;
			if (dot < smallestDot)
			{
				tempPos = overlap - acc;
				smallestDot = dot;
			}
			vel = v;
		}
		pos += tempPos; // (static_cast<float>(collisionStack.size())) ;
		//vel /= static_cast<float>(collisionStack.size());
	}


	if (!(vel.x == 0 || vel.y == 0)) {
		if (magsq(vel) < 0.01 && !always_active) {
			vel = sf::Vector2f(0, 0);
		}
	} else
	{
		if (magsq(acc) <= 0 && !always_active) {
			active = false;
		}
	}

	//std::cout << acc.x << " , " << acc.y << "\n" << delta_time << "\n\n";


	//std::cout << vel_mag << "\n";
	//if(!collision)
	vel += acc * delta_time;
	const auto vel_mag = magsq(vel);
	if (vel_mag > max_vel)
	{
		vel = (vel / sqrt(vel_mag)) * sqrt_max_vel;
	}
	pos += vel * delta_time;
	//collision = false;

	vel *= (1 - friction * delta_time);
	//rad = sqrt(vel_mag) + 5.f;
	//mass = rad * rad * 3.14156f;

	//std::cout << delta_time << "\n";

	collisionStack.clear();
}



void Ball::draw(sf::RenderWindow& window) const
{
	sf::CircleShape circ(rad);
	circ.setPosition(pos - sf::Vector2f(rad, rad));
	circ.setFillColor(sf::Color::Transparent);
	
	circ.setOutlineColor(sf::Color::White);
	circ.setOutlineThickness(-3);
	window.draw(circ);

	//sf::RectangleShape rect(sf::Vector2f(rad, 3));
	//rect.setRotation(atan2f(vel.y, vel.x) * (180.f/3.14156f));
	//rect.setPosition(pos);
	//rect.setFillColor(sf::Color::Transparent);
	//rect.setOutlineColor(sf::Color::White);
	//rect.setOutlineThickness(-3);
	//window.draw(rect);

	//sf::Font font;
	//font.loadFromFile("COMIC.TTF");

	//sf::Text text;
	//text.setFont(font);
	//text.setPosition(pos - sf::Vector2f(rad, rad));
	//text.setCharacterSize(24);
	//text.setFillColor(sf::Color::White);
	//text.setString(std::to_string(id));
	//window.draw(text);
}

//void Ball::draw(sf::RenderWindow window)
//{
//	sf::CircleShape circ(rad);
//	circ.setFillColor(sf::Color::Transparent);
//	circ.setOutlineColor(sf::Color::White);
//}

float Ball::magsq(sf::Vector2f v) {
	return v.x * v.x + v.y * v.y;
}

bool Ball::isInside(sf::Vector2f v) const
{
	return (magsq(v - pos) < rad * rad);
}

bool Ball::circlesOverlap(sf::Vector2f pos1, float rad1, sf::Vector2f pos2, float rad2)
{
	return magsq((pos1 - pos2)) <= (rad1 + rad2) * (rad1 + rad2);
}

Collision Ball::collide(Ball& ball1, Ball& ball2) {
	
	/*if (ball1 == NULL || ball2 ==)
		return Collision();*/
	if (ball1.mass > ball2.mass) {
		return collide(ball2, ball1);
	}

	if (!(ball1.active || ball2.active || ball1.always_active || ball2.always_active))
		return Collision();
	

	Collision out;
	out.collision = false;
	if (ball1.id != ball2.id) {
		out.ball1 = ball1.id;
		out.ball2 = ball2.id;
		const sf::Vector2f ball1_vec = ball1.pos + ball1.vel + ball1.acc;
		const sf::Vector2f ball2_vec = ball2.pos + ball2.vel + ball2.acc;
		if (circlesOverlap(ball1_vec, ball1.rad, ball2_vec, ball2.rad)) {
			ball1.active = true;
			ball2.active = true;
			//distance between ball centers
			//ball1.collision = true;
			//ball2.collision = true;
			out.collision = true;
			float d = sqrt(magsq((ball1_vec) - (ball2_vec)));

			/*if(ball1.collision || ball2.collision)
			{
				ball1.collision = true;
				ball2.collision = true;
			}*/

			const float overlap = 0.5f * (d - ball1.rad - ball2.rad);
			if (d > 0) {
				out.ball1_overlap = -1.1f * overlap * ((ball1_vec)-(ball2_vec)) / d;
				out.ball2_overlap = -1.1f * overlap * ((ball2_vec)-(ball1_vec)) / d;
			} else
			{

				out.ball1_overlap = {0, 0};
				out.ball2_overlap = {0, 0};
			}
			out.dist = d;
		}
	}
	return out;
}
