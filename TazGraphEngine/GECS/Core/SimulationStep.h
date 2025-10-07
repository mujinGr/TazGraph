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

	struct TazSimulationPath {
		TazColor color;
		float width;
		std::vector<EntityID> link_ids;
	};

	sim_dump::UInt32 step_index;
	double timestamp;

	// node data
	std::vector<std::pair<NodeEntity*, TazSimulationNode>> nodes;

	// link data
	std::vector< std::pair<LinkEntity*, TazSimulationLink>>  links;

	// path data
	std::vector<std::pair<EmptyEntity*, TazSimulationPath>> paths; // pathId → (color, width)
};
