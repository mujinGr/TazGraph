
#include "App/App.h"


int main(int argc, char* argv[]) {
	
	int threadCount = 4; // Default

	if (argc > 1) {
		threadCount = std::stoi(argv[1]); // Parse first argument
		if (threadCount <= 0) {
			std::cerr << "Invalid thread count. Must be > 0.\n";
			return 1;
		}
	}

	App app(threadCount);

	app.run();

	return 0;
}
