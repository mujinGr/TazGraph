
#include "App/App.h"


int main(int argc, char* argv[]) {

	int threadCount = 4; // Default
	std::string openFile = "";
	double initialTimestamp = 0.0;
	int initialStep = 0;

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
		else if (arg.find("--open-file=") == 0) {
			openFile = arg.substr(12); // Length of "--open-file="
		}
		else if (arg.find("--initial-timestamp=") == 0) {
			initialTimestamp = std::stod(arg.substr(20)); // Length of "--initial-timestamp="
		}
		else if (arg.find("--initial-step=") == 0) {
			initialStep = std::stoi(arg.substr(15)); // Length of "--initial-step="
		}
		else {
			std::cerr << "Unknown argument: " << arg << "\n";
			std::cerr << "Usage: " << argv[0] << " [--num-threads=X] [--open-file=Y] [--initial-timestamp=Z] [--initial-step=W]\n";
			return 1;
		}
	}

	// Display parsed values (optional)
	std::cout << "Thread count: " << threadCount << "\n";
	if (!openFile.empty()) {
		std::cout << "Opening file: " << openFile << "\n";
	}
	if (initialTimestamp != 0.0) {
		std::cout << "Initial timestamp: " << initialTimestamp << "\n";
	}
	if (initialStep != 0) {
		std::cout << "Initial step: " << initialStep << "\n";
	}

	auto app = std::make_unique<App>(threadCount, openFile, initialTimestamp, initialStep);
	app->run();

	return 0;
}
