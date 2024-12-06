#pragma once
#include <vector>

class IMainGame;
class IGameScreen;

class ScreenList {
public:
	ScreenList(IMainGame* game);
	~ScreenList();

	IGameScreen* moveNext();
	IGameScreen* movePrevious();

	void setScreen(int nextScreen);
	void addScreen(IGameScreen* newScreen);

	void destroy();

	IGameScreen* getCurrent();

protected:
	IMainGame* _game = nullptr;
	std::vector<IGameScreen*> _screens;
	int _currentScreenIndex = -1;
};