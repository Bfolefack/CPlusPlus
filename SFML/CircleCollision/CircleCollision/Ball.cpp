#include "Ball.h"
#include "Collision.h"

#include <cmath>
#include <string>
#include <unordered_map>
#include <iostream>

using std::shared_ptr;

 std::mutex Ball::universal_ball_mutex;

Ball::Ball(int i, float r, float x, float y, float vx, float vy, float m, float e) {
	id = i;
	rad = r;
	pos = sf::Vector2f(x, y);
	//vel = sf::Vector2f((((rand() % 2) - 0.5f) * 1.f), (((rand() % 2 - 0.5f)) * 1.f));
	//acc = sf::Vector2f(((rand() % 2 - 0.5f) * 0.5f), ((rand() % 2 - 0.5f) * 0.5f));
	//vel = sf::Vector2f(vx, vy);
	//acc = sf::Vector2f(0, 0);
	mass = r * r * 3.14156f;
	elasticity = e;
	collisionStack = std::unordered_map<int, std::tuple<float, sf::Vector2f, sf::Vector2f>>();
	mutex = std::make_shared<std::mutex>();
}

void Ball::update()
{
	//if (!collision)
	//acc = sf::Vector2f(0, 0);
	//std::unique_lock<std::mutex> lock(*mutex);
	//auto start = std::chrono::high_resolution_clock::now();
	//if (lock.try_lock()) {
		if (!collisionStack.empty()) {
			vel = sf::Vector2f(0, 0);
			//auto ogVel = vel;
			auto tempPos = sf::Vector2f(0, 0);
			float smallestDot = 50;
			if (collisionStack.empty())
			{
				std::cout << "empty" << std::endl;
			}
			for (std::pair<const int, std::tuple<float, sf::Vector2<float>, sf::Vector2<float>>> i : collisionStack) {
				delta_time = std::get<0>(i.second);
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
		else
		{
			collision = false;
		}
		const auto vel_mag = magsq(vel);

		if (vel_mag < magsq(acc) / 2.f) {
			vel = sf::Vector2f(0, 0);
		}

		//std::cout << acc.x << " , " << acc.y << "\n" << delta_time << "\n\n";


		//std::cout << vel_mag << "\n";
		//if(!collision)
		vel += acc;
		if (vel_mag > max_vel)
		{
			vel = (vel / sqrt(vel_mag)) * sqrt_max_vel;
		}
		pos += vel * delta_time;
		//collision = false;
		vel *= .999f;



		collisionStack.clear();
		//auto end = std::chrono::high_resolution_clock::now();
		//std::cout << "Ball Update: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
	//}
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
	return 1.01 * magsq((pos1 - pos2)) <= (rad1 + rad2) * (rad1 + rad2);
}

Collision Ball::collide(const shared_ptr<Ball> ball1, const shared_ptr<Ball> ball2) {
	shared_ptr<Ball> b1;
	shared_ptr<Ball> b2;
	if (ball1 == nullptr || ball2 == nullptr)
		return Collision();
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
		const sf::Vector2f b1_vec = b1->pos + b1->vel + b1->acc;
		const sf::Vector2f b2_vec = b2->pos + b2->vel + b2->acc;
		if (circlesOverlap(b1_vec, b1->rad, b2_vec, b2->rad)) {
			//distance between ball centers
			//b1->collision = true;
			//b2->collision = true;
			out.collision = true;
			float d = sqrt(magsq((b1_vec) - (b2_vec)));

			/*if(b1->collision || b2->collision)
			{
				b1->collision = true;
				b2->collision = true;
			}*/
			
			float overlap = 0.5f * (d - b1->rad - b2->rad);
			out.ball1_overlap = -1.1f * overlap * ((b1_vec) - (b2_vec)) / d;
			out.ball2_overlap = -1.1f * overlap * ((b2_vec) - (b1_vec)) / d;
			out.dist = d;
		}
	}
	return out;
}
