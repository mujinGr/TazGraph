#include "ScreenList.h"
#include "IGameScreen.h"

ScreenList::ScreenList(IMainGame* game) :
	_game(game) {

}
ScreenList::~ScreenList() {
	destroy();
}

IGameScreen* ScreenList::moveNext() {
	IGameScreen* currentScreen = getCurrent();
	if (currentScreen->getNextScreenIndex() != SCREEN_INDEX_NO_SCREEN) {
		_currentScreenIndex = currentScreen->getNextScreenIndex();
	}
	return getCurrent();
}
IGameScreen* ScreenList::movePrevious() {
	IGameScreen* currentScreen = getCurrent();
	if (currentScreen->getPreviousScreenIndex() != SCREEN_INDEX_NO_SCREEN) {
		_currentScreenIndex = currentScreen->getPreviousScreenIndex();
	}
	return getCurrent();
}

void ScreenList::setScreen(int nextScreen) {
	_currentScreenIndex = nextScreen;
}
void ScreenList::addScreen(IGameScreen* newScreen) {
	newScreen->_screenIndex = _screens.size();
	_screens.push_back(newScreen);
	newScreen->build();
	newScreen->setParentGame(_game);
}

void ScreenList::destroy() {
	for (size_t i = 0; i < _screens.size(); i++) {
		_screens[i]->destroy();
	}
	_screens.resize(0);
	_currentScreenIndex = SCREEN_INDEX_NO_SCREEN;
}

IGameScreen* ScreenList::getCurrent() {
	if (_currentScreenIndex == SCREEN_INDEX_NO_SCREEN) return nullptr;

	return _screens[_currentScreenIndex];
}