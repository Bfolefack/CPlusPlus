#include "SFML/Window.hpp"
#include <Windows.h>
#include <chrono>
#include <iostream>
#include "BouncingWindow.h"

using std::unique_ptr;
using std::make_unique;
using std::vector;

unique_ptr <BouncingWindow> createBouncingWindow(int speed, int size) {
	BouncingWindow bw = BouncingWindow(speed, size);
	unique_ptr <BouncingWindow> bouncingwindow = make_unique<BouncingWindow>(bw);
	return move(bouncingwindow);
};

int main() {
	printf("Starting");
	srand((std::chrono::system_clock::now().time_since_epoch()).count());
	
	vector<unique_ptr <BouncingWindow>> vec;
	vec.push_back(createBouncingWindow(100, 400));
	
	while (true) {
		for (int i = 0; i < vec.size(); i++) {
			if ((*vec[i]).bounce()) {
				vec.push_back(createBouncingWindow(100, 400));
				vec.push_back(createBouncingWindow(100, 400));
			}
		}
		//Sleep(20);
	}
	return 0;
};

