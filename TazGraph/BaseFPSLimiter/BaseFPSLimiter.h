#pragma once
#include <deque>
class BaseFPSLimiter {
public:
	static constexpr int FPS_HISTORY_COUNT = 100;

	BaseFPSLimiter();

	void init(float maxFPS);

	void setMaxFPS(float maxFPS);

	void begin();

	//end will return the current FPS
	float end();
	
	float fps;
	float maxFPS;
	float frameTime;
	unsigned int startTicks;

	std::deque<float> fpsHistory;
	int fpsHistoryIndx = 0;

	void setHistoryValue(float currentFPS);
private:
	void calculateFPS();
	
};