#pragma once

#include "../pch.h"
#include <fstream>
#include <mutex>

namespace TazGraphEngine {
	class ConsoleLogger {
	public:
		static void log(const std::string& message) {
			printCurrentTime();
			std::cout << " " << message << std::endl;
		}

		static void error(const std::string& errorMessage) {
			printCurrentTime();
			std::cerr << " [ERROR] " << errorMessage << std::endl;
			std::abort();
		}

		static void writeToFile(const std::string& line) {
			std::lock_guard<std::mutex> lock(fileLog_mutex);

			std::ofstream file("file_log.txt",
				std::ios::out | std::ios::app);

			if (file.is_open()) {
				file << line << std::endl;
				file.flush(); // CRITICAL for crash debugging
			}
		}

	private:
		static inline std::mutex fileLog_mutex;

		static void printCurrentTime() {
			std::time_t now = std::time(nullptr);
			struct tm timeInfo;

#if defined(_WIN32) || defined(_WIN64)
			localtime_s(&timeInfo, &now);
#else
			localtime_r(&now, &timeInfo);
#endif

			char timestamp[20]; // Sufficient space for the timestamp
			strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", &timeInfo);

			std::cout << timestamp;
		}
	};
}