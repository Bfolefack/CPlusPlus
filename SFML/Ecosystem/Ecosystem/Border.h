#pragma once

struct Border {
public:
	Border() {
		left = false;
		right = false;
		top = false;
		bottom = false;
	}
	Border(bool l, bool r, bool t, bool b) {
		left = l;
		right = r;
		top = t;
		bottom = b;
	}
	bool left;
	bool right;
	bool top;
	bool bottom;
};