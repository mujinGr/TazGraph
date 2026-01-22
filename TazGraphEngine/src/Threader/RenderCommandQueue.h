#pragma once
#include <vector>
#include <mutex>
#include <functional>

struct RenderCommandQueue {
	std::vector<std::function<void()>> commands;
	std::mutex mutex;
	std::condition_variable cv;
	bool isReady = false;

	void Submit(std::function<void()> cmd) {
		std::lock_guard<std::mutex> lock(mutex);
		commands.push_back(std::move(cmd));
		isReady = true;
	}

	void Execute() {
		std::lock_guard<std::mutex> lock(mutex);
		for (auto& cmd : commands) cmd();
		commands.clear();
		cv.notify_all();
		isReady = false;
	}

	void Wait() {
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [this]() {
			return commands.empty();
			});
	}
};
