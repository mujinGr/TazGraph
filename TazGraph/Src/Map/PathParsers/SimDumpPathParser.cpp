#include "./TextPathParser.h"
#include "SimDumpPathParser.h"

SimDumpPathParser::SimDumpPathParser() {}

void SimDumpPathParser::readFile(std::string m_fileName) {
}

void SimDumpPathParser::writeFile(std::string m_fileName, Manager& manager)
{
}


void SimDumpPathParser::parse(Manager& manager,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc)
{

	manager.updateInnerPathLinks = true;

}

void SimDumpPathParser::parse(Manager& manager,
	sim_dump::FileReader& reader,
	std::vector<NodeEntity*>& nodeEntities,
	std::vector<LinkEntity*>& linkEntities,
	std::function<void(Entity&, glm::vec3)> addNodeFunc,
	std::function<void(Entity&)> addLinkFunc)
{
	auto path_it = reader.get_path_iterator();
	auto path_end = reader.get_path_end();

	// Iterate over all paths in the current timestep
	for (; path_it != path_end; ++path_it) {
		const auto& pathData = path_it->second;

		// Check entities exist for all links in path
		std::vector<int> missingIds;
		for (sim_dump::UInt32 linkId : pathData.links) {
			if (!manager.hasEntity((int)linkId)) {
				missingIds.push_back(linkId);
			}
		}

		if (!missingIds.empty()) {
			std::cerr << "Skipping path " << pathData.id << " due to missing link IDs: ";
			for (int mid : missingIds) std::cerr << mid << " ";
			std::cerr << std::endl;
			continue;
		}


		//todo links show literally the link ids of the ones parsed and not the node ids
		if (pathData.links.size() < 2) continue;
		// Create a path linker entity
		auto& pathLinker = manager.addEntity<Empty>();
		auto& plc = pathLinker.addComponent<PathLinkerComponent>();

		// Assign color + width
		plc.color = TazColor(
			pathData.color.r,
			pathData.color.g,
			pathData.color.b,
			pathData.color.alpha
		);
		plc.width = pathData.width;

		// Create link entities for the path
		for (size_t i = 0; i < pathData.links.size(); ++i) {

			sim_dump::UInt32 idA = std::get<int>(linkEntities[i]->getFromNode()->getId());
			sim_dump::UInt32 idB = std::get<int>(linkEntities[i]->getToNode()->getId());

			// create ECS link
			auto& link = manager.addEntity<Link>((int)idA, (int)idB);
			link.addGroup(Manager::groupPathLinks);
			addLinkFunc(link);

			// associate link with path linker
			pathLinker.GetComponent<PathLinkerComponent>().addLink(&link);
			pathLinker.addGroup(Manager::groupPathLinksHolder);
		}
	}

	// Register links in grid system
	for (auto& link : manager.getGroup<LinkEntity>(Manager::groupPathLinks)) {
		manager.grid->addLink(link, manager.grid->getGridLevel());
	}

	manager.updateInnerPathLinks = true;

}

void SimDumpPathParser::closeFile()
{
}
