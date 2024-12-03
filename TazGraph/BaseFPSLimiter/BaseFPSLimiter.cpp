#include "BaseFPSLimiter.h"

#include <SDL/SDL.h>

BaseFPSLimiter::BaseFPSLimiter() {

}

void BaseFPSLimiter::init(float mmaxFPS) {
	setMaxFPS(mmaxFPS);
}

void BaseFPSLimiter::setMaxFPS(float mmaxFPS) {
	maxFPS = mmaxFPS;
}

void BaseFPSLimiter::begin() {
	startTicks = SDL_GetTicks();
}

float BaseFPSLimiter::end() {

	calculateFPS();

	float frameTicks = SDL_GetTicks() - startTicks;

	if (1000.0f / maxFPS > frameTicks)
	{
		SDL_Delay((Uint32)(1000.0f / maxFPS - frameTicks));
	}

	return fps;
}

void BaseFPSLimiter::setHistoryValue(float currentFPS)
{
	fpsHistory[fpsHistoryIndx % fps_history_count] = fps;
	fpsHistoryIndx++;
}

void BaseFPSLimiter::calculateFPS() {
	static const int NUM_SAMPLES = 10;

	static float frameTimes[NUM_SAMPLES];

	static int currentFrame = 0;

	static float prevTicks = SDL_GetTicks();

	float currentTicks = SDL_GetTicks();

	frameTime = currentTicks - prevTicks;
	frameTimes[currentFrame % NUM_SAMPLES] = frameTime;

	prevTicks = currentTicks;

	int count;

	currentFrame++;
	if (currentFrame < NUM_SAMPLES) {
		count = currentFrame;
	}
	else {
		count = NUM_SAMPLES;
	}

	float frameTimeAverage = 0;
	for (int i = 0; i < count; i++) {
		frameTimeAverage += frameTimes[i];
	}
	frameTimeAverage /= count;

	//Calculate FPS
	if (frameTimeAverage > 0) {
		fps = 1000.0f / frameTimeAverage;
	}
	else {
		fps = 60.0f;
	}
}