#pragma once
#include <vector>

class IMainGraph;
class IGraphScreen;

class ScreenList {
public:
	ScreenList(IMainGraph* game);
	~ScreenList();

	IGraphScreen* moveNext();
	IGraphScreen* movePrevious();

	void setScreen(int nextScreen);
	void addScreen(IGraphScreen* newScreen);

	void destroy();

	IGraphScreen* getCurrent();

protected:
	IMainGraph* _graph = nullptr;
	std::vector<IGraphScreen*> _screens;
	int _currentScreenIndex = -1;
};