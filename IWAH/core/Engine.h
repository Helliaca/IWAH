#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include <vector>

class Engine
{
private:
public:
	std::vector<int> rules;
	std::vector<int> flips;
	int maxlit;
	void run();
	void parse();

	Engine();
	~Engine();
};

#endif