#include "TimedSplashScreen.h"


TimedSplashScreen::TimedSplashScreen(int _width, int _height, int _millis, std::string _fileName) :
	width(_width),
	height(_height),
	millis(_millis),
	fileName(_fileName)
{
	window = SDL_CreateWindow("ElectroDynamic",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_BORDERLESS);
	renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
	SDL_Surface *surface = SDL_LoadBMP(fileName.c_str());
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer,surface);
	SDL_RenderClear(renderer);
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = width;
	rect.h = height;
	SDL_RenderCopy(renderer,tex,NULL,&rect);
	SDL_RenderPresent(renderer);
	SDL_Delay(millis);
	SDL_DestroyTexture(tex);
	SDL_FreeSurface(surface);
}


TimedSplashScreen::~TimedSplashScreen(void)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}
