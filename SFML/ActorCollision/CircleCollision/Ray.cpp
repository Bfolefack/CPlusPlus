#include "Ray.h";

#include "Actor.h"


bool Ray::hit(sf::Vector2f source, float angle, float range, sf::Vector2f point, float rad)
{
	if(source == point)
	{
		return false;
	}
	auto x1 = source.x - point.x;
	auto y1 = source.y - point.y;
	float x2 = x1 + range * cos(angle);
	float y2 = y1 + range * sin(angle);
	auto dr_sq = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	auto D = x1 * y2 - x2 * y1;
	float hit = rad * rad * dr_sq - D * D;
	return hit >= 0;
	//auto dr_sq = 
}

int Ray::hit(sf::Vector2f source, float angle, float range, std::vector<std::shared_ptr<Actor>>& actors)
{
	float dist = range * range;
	int out = -1;
	int count = 0;
	for(const auto& a : actors)
	{
		if(hit(source, angle, range, a->ball.pos, a->ball.rad))
		{
			auto d = (source.x - a->ball.pos.x) * (source.x - a->ball.pos.x) + (source.y - a->ball.pos.y) * (source.y - a->ball.pos.y);
			if(d < dist)
			{
				out = count;
				dist = d;
			}
		}
		count++;
	}
	return out;
}
