#pragma once

#include "../../GLSLProgram.h"
#include <simdump/include/sim_dump/datatypes.h>
#include "./GECSEntity.h"

struct SimulationStep {

	struct TazSimulationNode {
		TazPosition position;
		TazColor color;
		TazSize size;
	};

	struct TazSimulationLink : LineInstanceData {
	};

	sim_dump::UInt32 step_index;
	sim_dump::UInt64 timestamp;

	// node data
	std::vector<std::pair<NodeEntity*, TazSimulationNode>> nodes;

	// link data
	std::vector< std::pair<LinkEntity*, TazSimulationLink>>  links;

	// path data
	std::unordered_map<int, std::pair<TazColor, float>> paths; // pathId → (color, width)
};
