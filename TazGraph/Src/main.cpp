
#include "App/App.h"

//#include <pybind11/pybind11.h>
//#include <pybind11/embed.h>
//
//namespace py = pybind11;
//
//static std::unique_ptr<py::scoped_interpreter> pythonRuntime;

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

std::array<float, 4> parseColorRGBA(const std::string& s)
{
	std::stringstream ss(s);
	float r, g, b, a = 1.0f;
	char comma;

	if (!(ss >> r >> comma >> g >> comma >> b)) {
		throw std::invalid_argument("Invalid color format");
	}

	if (ss >> comma >> a) {
		// RGBA provided
	}

	return { r, g, b, a };
}
#ifdef TAZ_PLATFORM_WINDOWS
bool findDll(const char* dllName) {
	HMODULE hModule = GetModuleHandleA(dllName);
	if (!hModule) {
		std::cout << dllName << " is NOT loaded" << std::endl;
		return false;
	}
	std::cout << dllName << " is LOADED at address: " << hModule << std::endl;
	char dllPath[MAX_PATH];
	if (GetModuleFileNameA(hModule, dllPath, MAX_PATH)) {
		std::cout << dllName << " is loaded from: " << dllPath << std::endl;
	}
	else {
		std::cout << dllName << " is loaded but path not available" << std::endl;
	}
	return true;
}

void checkAllDlls() {
	// Check all loaded modules
	std::cout << "\n=== All loaded Python-related DLLs ===" << std::endl;

	HMODULE hModules[1024];
	DWORD cbNeeded;
	if (EnumProcessModules(GetCurrentProcess(), hModules, sizeof(hModules), &cbNeeded)) {
		for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
			char moduleName[MAX_PATH];
			if (GetModuleFileNameA(hModules[i], moduleName, sizeof(moduleName))) {
				std::string name = moduleName;
				if (name.find("python") != std::string::npos ||
					name.find("Python") != std::string::npos) {
					std::cout << "  " << name << std::endl;
				}
			}
		}
	}

}

#endif // TAZ_PLATFORM_WINDOWS


int main(int argc, char* argv[]) {
	TazGraphEngine::ConsoleLogger::init();

	int threadCount = 4; // Default
	int msaaSamples = 1;
	std::string openFile = "";
	double initialTimestamp = 0.0;
	int initialStep = 0;
	bool usePython = true;
	std::array<float, 4> backgroundColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	bool useGrid = true;
#ifdef TAZ_PLATFORM_WINDOWS

	bool pythonLoaded = findDll("python313.dll");
	bool python3Loaded = findDll("python3.dll");

	checkAllDlls();
#endif // TAZ_PLATFORM_WINDOWS


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
			msaaSamples = std::stoi(arg.substr(7)); // Length of "--MSAA="
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
		else if (arg.find("--bg-color=") == 0) {
			std::array<float, 4> parsedColor = parseColorRGBA(arg.substr(11)); // Length of "--bg-color="
			backgroundColor = parsedColor;
		}
		else if (arg.find("--grid=") == 0) {
			useGrid = stringToBool(arg.substr(7)); // Length of "--grid="
		}
		else {
			std::cerr << "Unknown argument: " << arg << "\n";
			std::cerr << "Usage: " << argv[0] << R"(
 [--num-threads=X]
 [--MSAA=V]
 [--open-file=Y]
 [--initial-timestamp=Z]
 [--initial-step=W]
 [--use-python=C]
 [--bg-color=FR,FG,FB,FA]
 [--grid=G]
)";
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
	{
		std::ostringstream oss;
		oss << "Background Color: ("
			<< backgroundColor[0] << ", "
			<< backgroundColor[1] << ", "
			<< backgroundColor[2] << ", "
			<< backgroundColor[3] << ")";
		TAZ_LOG(oss.str());
	}
	{
		std::ostringstream oss;
		oss << "Use Grid: " << (useGrid ? "true" : "false");
		TAZ_LOG(oss.str());
	}
	TAZ_LOG("\n\n");

	auto app = std::make_unique<App>(
		threadCount,
		msaaSamples,
		openFile,
		initialTimestamp,
		initialStep,
		usePython,
		backgroundColor,
		useGrid);


	app->run();

	/*if (!pythonRuntime)
		pythonRuntime = std::make_unique<py::scoped_interpreter>();


	char _pythonBuffer[1024] = "print(123213412)";
	std::string _outputText;

	py::exec(R"(
				import sys
				from io import StringIO
				sys.stdout = StringIO()
			)");

	py::module_ userapi = py::module_::create_extension_module("tazpyapi", nullptr, new PyModuleDef{});
	py::module_::import("sys").attr("modules")["tazpyapi"] = userapi;

	py::exec(_pythonBuffer);
	py::object output = py::eval("sys.stdout.getvalue()");
	_outputText = output.cast<std::string>();

	std::cout << _outputText << std::endl;*/

	return 0;
}
