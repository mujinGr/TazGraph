
#include "App/App.h"

bool stringToBool(const std::string& str) {
	std::string lowerStr = str;
	std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
		[](unsigned char c) { return std::tolower(c); });

	if (lowerStr == "1" || lowerStr == "true" || lowerStr == "yes" ||
		lowerStr == "on" || lowerStr == "enable" || lowerStr == "enabled") {
		return true;
	}
	if (lowerStr == "0" || lowerStr == "false" || lowerStr == "no" ||
		lowerStr == "off" || lowerStr == "disable" || lowerStr == "disabled") {
		return false;
	}
	return false;
}

int SDL_main(int argc, char* argv[]) {
	TazGraphEngine::ConsoleLogger::init();

	int threadCount = 4; // Default
	int msaaSamples = 1;
	std::string openFile = "";
	double initialTimestamp = 0.0;
	int initialStep = 0;
	bool usePython = false;

	// Parse command line arguments
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];

		if (arg.find("--num-threads=") == 0) {
			threadCount = std::stoi(arg.substr(14)); // Length of "--num-threads="
			if (threadCount <= 0) {
				std::cerr << "Invalid thread count. Must be > 0.\n";
				return 1;
			}
		}
		else if (arg.find("--MSAA=") == 0) {
			msaaSamples = std::stoi(arg.substr(7)); // Length of "--initial-step="
		}
		else if (arg.find("--open-file=") == 0) {
			openFile = arg.substr(12); // Length of "--open-file="
		}
		else if (arg.find("--initial-timestamp=") == 0) {
			initialTimestamp = std::stod(arg.substr(20)); // Length of "--initial-timestamp="
		}
		else if (arg.find("--initial-step=") == 0) {
			initialStep = std::stoi(arg.substr(15)); // Length of "--initial-step="
		}
		else if (arg.find("--use-python=") == 0) {
			usePython = stringToBool(arg.substr(13)); // Length of "--use-python="
		}
		else {
			std::cerr << "Unknown argument: " << arg << "\n";
			std::cerr << "Usage: " << argv[0] << " [--num-threads=X] [--open-file=Y] [--initial-timestamp=Z] [--initial-step=W] [--MSAA=V] [--use-python=C]\n";
			return 1;
		}
	}

	// Display parsed values (optional)
	TAZ_LOG("Thread count: " + std::to_string(threadCount));
	if (!openFile.empty()) {
		TAZ_LOG("Opening file: " + openFile);
	}
	{
		TAZ_LOG("MSAA: " + std::to_string(msaaSamples));
	}
	if (initialTimestamp != 0.0) {
		TAZ_LOG("Initial timestamp: " + std::to_string(initialTimestamp));
	}
	if (initialStep != 0) {
		TAZ_LOG("Initial step: " + std::to_string(initialStep));
	}
	{
		std::ostringstream oss;
		oss << "Use Python: " << (usePython ? "true" : "false");
		TAZ_LOG(oss.str());
	}
	auto app = std::make_unique<App>(threadCount, msaaSamples, openFile, initialTimestamp, initialStep, usePython);
	app->run();

	return 0;
}
