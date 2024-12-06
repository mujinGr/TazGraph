#include "ScreenList.h"
#include "IGraphScreen.h"

ScreenList::ScreenList(IMainGraph* graph) :
	_graph(graph) {

}
ScreenList::~ScreenList() {
	destroy();
}

IGraphScreen* ScreenList::moveNext() {
	IGraphScreen* currentScreen = getCurrent();
	if (currentScreen->getNextScreenIndex() != SCREEN_INDEX_NO_SCREEN) {
		_currentScreenIndex = currentScreen->getNextScreenIndex();
	}
	return getCurrent();
}
IGraphScreen* ScreenList::movePrevious() {
	IGraphScreen* currentScreen = getCurrent();
	if (currentScreen->getPreviousScreenIndex() != SCREEN_INDEX_NO_SCREEN) {
		_currentScreenIndex = currentScreen->getPreviousScreenIndex();
	}
	return getCurrent();
}

void ScreenList::setScreen(int nextScreen) {
	_currentScreenIndex = nextScreen;
}
void ScreenList::addScreen(IGraphScreen* newScreen) {
	newScreen->_screenIndex = _screens.size();
	_screens.push_back(newScreen);
	newScreen->build();
	newScreen->setParentGraph(_graph);
}

void ScreenList::destroy() {
	for (size_t i = 0; i < _screens.size(); i++) {
		_screens[i]->destroy();
	}
	_screens.resize(0);
	_currentScreenIndex = SCREEN_INDEX_NO_SCREEN;
}

IGraphScreen* ScreenList::getCurrent() {
	if (_currentScreenIndex == SCREEN_INDEX_NO_SCREEN) return nullptr;

	return _screens[_currentScreenIndex];
}