#pragma once

#include "destructive_reasoning.h"

class TimedSplashScreen
{
public:
	TimedSplashScreen(int width, int height, int millis, std::string fileName);
	~TimedSplashScreen(void);

private:
	int width;
	int height;
	int millis;
	std::string fileName;

	SDL_Window *window;
	SDL_Renderer *renderer;
};

