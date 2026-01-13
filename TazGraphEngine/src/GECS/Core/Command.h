#pragma once

#include "pch.h"

class Command {
public:
	using Callback = std::function<void()>;

	Command(const std::string& name, Callback executeFunc, Callback undoFunc = nullptr)
		: _name(name), _executeFunc(executeFunc), _undoFunc(undoFunc) {
	}

	void execute() {
		if (_executeFunc) {
			_executeFunc();
		}
	}

	void undo() {
		if (_undoFunc) {
			_undoFunc();
		}
	}

	const std::string& getName() const { return _name; }
	bool canUndo() const { return _undoFunc != nullptr; }

private:
	std::string _name;
	Callback _executeFunc;
	Callback _undoFunc;
};