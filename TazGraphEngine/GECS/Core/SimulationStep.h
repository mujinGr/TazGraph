#pragma once

#include "../../Vertex.h"
#include <simdump/include/sim_dump/datatypes.h>

struct SimulationStep {
	sim_dump::UInt32 step_index;
	sim_dump::UInt64 timestamp;

	// node data
	std::vector<glm::vec3> positions;
	std::vector<TazColor>  nodeColors;
	std::vector<float>     nodeSizes;

	// link data
	std::vector<TazColor>  linkColors;
	std::vector<float>     linkWidths;

	// path data
	std::unordered_map<int, std::pair<TazColor, float>> paths; // pathId → (color, width)
};
