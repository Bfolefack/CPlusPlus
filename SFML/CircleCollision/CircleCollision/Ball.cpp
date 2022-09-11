#include "Ball.h"
#include <cmath>
#include <string>
#include <unordered_map>
#include <iostream>

using std::shared_ptr;

Ball::Ball(int i, float r, float x, float y, float vx, float vy, float m, float e) {
	id = i;
	rad = r;
	pos = sf::Vector2f(x, y);
	//vel = sf::Vector2f((rand() % 100 - 50) / 50.f, (rand() % 100 - 50) / 50.f);
	//acc = sf::Vector2f((rand() % 100 - 50)/50.f, 0);
	acc = sf::Vector2f(vx, vy);
	mass = r * r * 3.14156f;
	elasticity = e;
	collisionStack = std::unordered_map<int, std::pair<sf::Vector2f, sf::Vector2f>>();
}

void Ball::update()
{
	//if (!collision)
	//acc = sf::Vector2f(0, 0);
	if (!collisionStack.empty()) {
		vel = sf::Vector2f(0, 0);
		//auto ogVel = vel;
		auto tempPos = sf::Vector2f(0, 0);
		float smallestDot = 50;
		for (auto i : collisionStack) {
			auto dot = i.second.first.x * acc.x + i.second.first.y * acc.y;
			if(dot < smallestDot)
			{
				tempPos = i.second.first - acc;
				smallestDot = dot;
			}
			vel = i.second.second * delta_time;
		}
		pos += tempPos; // (static_cast<float>(collisionStack.size())) ;
		//vel /= static_cast<float>(collisionStack.size());
	} else
	{
		collision = false;
	}
	auto vel_mag = magsq(vel);
	if (vel_mag > max_vel)
	{
		vel = (vel / sqrt(vel_mag)) * sqrt_max_vel;
	}
	if (vel_mag < magsq(acc)/2.f) {
		vel = sf::Vector2f(0, 0);
	}

	//std::cout << acc.x << " , " << acc.y << "\n" << delta_time << "\n\n";

	
	//std::cout << vel_mag << "\n";
	//if(!collision)
	vel += acc * delta_time;
	pos += vel;
	//collision = false;
	vel *= .975f;

	
	
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

	sf::RectangleShape rect(sf::Vector2f(rad, 3));
	rect.setRotation(atan2f(vel.y, vel.x) * (180.f/3.14156f));
	rect.setPosition(pos);
	rect.setFillColor(sf::Color::Transparent);
	rect.setOutlineColor(sf::Color::White);
	rect.setOutlineThickness(-3);
	window.draw(rect);

	/*sf::Font font;
	font.loadFromFile("COMIC.TTF");

	sf::Text text;
	text.setFont(font);
	text.setPosition(pos - sf::Vector2f(rad, rad));
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::White);
	text.setString(std::to_string(id));
	window.draw(text);*/
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
	return 1.01 * magsq((pos1 - pos2)) <= (rad1 + rad2) * (rad1 + rad2);
}

Collision Ball::collide(const shared_ptr<Ball> ball1, const shared_ptr<Ball> ball2) {
	shared_ptr<Ball> b1;
	shared_ptr<Ball> b2;
	if (ball1->id < ball2->id) {
		b1 = ball1;
		b2 = ball2;
	}
	else {
		b2 = ball1;
		b1 = ball2;
	}

	Collision out;
	out.collision = false;
	if (b1->id != b2->id) {
		out.ball1 = b1->id;
		out.ball2 = b2->id;
		sf::Vector2f b1Vec = b1->pos + b1->vel + b1->acc;
		sf::Vector2f b2Vec = b2->pos + b2->vel + b2->acc;
		if (circlesOverlap(b1Vec, b1->rad, b2Vec, b2->rad)) {
			//distance between ball centers
			//b1->collision = true;
			//b2->collision = true;
			out.collision = true;
			float d = sqrt(magsq((b1Vec) - (b2Vec)));

			/*if(b1->collision || b2->collision)
			{
				b1->collision = true;
				b2->collision = true;
			}*/
			
			float overlap = 0.5f * (d - b1->rad - b2->rad);
			out.ball1_overlap = -1.1f * overlap * ((b1Vec) - (b2Vec)) / d;
			out.ball2_overlap = -1.1f * overlap * ((b2Vec) - (b1Vec)) / d;
			out.dist = d;
		}
	}
	return out;
}
