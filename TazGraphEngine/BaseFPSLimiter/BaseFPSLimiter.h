#pragma once


class BaseFPSLimiter {
public:
	static constexpr int fps_history_count = 100;

	BaseFPSLimiter();

	void init(float maxFPS);

	void setMaxFPS(float maxFPS);

	void begin();

	//end will return the current FPS
	float end();

	float fpsHistory[fps_history_count] = { 0 };
	int fpsHistoryIndx = 0;

	float fps;
	float maxFPS;
	float frameTime;
	unsigned int startTicks;

	void setHistoryValue(float currentFPS);

private:
	void calculateFPS();

};