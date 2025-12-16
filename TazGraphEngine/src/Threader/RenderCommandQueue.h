#pragma once
#include <vector>
#include <mutex>
#include <functional>

struct RenderCommandQueue {
	std::vector<std::function<void()>> commands;
	std::mutex mutex;

	void Submit(std::function<void()> cmd) {
		std::lock_guard<std::mutex> lock(mutex);
		commands.push_back(std::move(cmd));
	}

	void Execute() {
		std::lock_guard<std::mutex> lock(mutex);
		for (auto& cmd : commands) cmd();
		commands.clear();
	}
};
