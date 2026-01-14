#pragma once

#include "../../pch.h"

#include "../GECS/Core/SimulationStep.h"
#include "../GECS/UtilComponents.h"

namespace fs = std::filesystem;

namespace Taz {
	struct GECSRenderBatch : RenderBatch {
		std::vector<Entity*> entities;
	};

	struct FrameRenderData {
		std::vector<GECSRenderBatch> batches;
		glm::vec4 backgroundColor;
		bool renderDebug = false;
	};
}


class DataManager {
public:
	// Gets the single instance of CameraManager (singleton)
	static DataManager& getInstance() {
		static DataManager instance; // Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

	DataManager() {}

	ImGui::ComboAutoSelectData data;
	ImGui::ComboAutoSelectData pathData;

	std::string mapToLoad;

	std::vector<std::string> fileNames;
	std::vector<std::string> pollingFileNames;
	std::vector<std::string> pathsFileNames;
	std::string pathLoading;

	bool filesLoaded = false;

	bool saving = false;
	bool startingNew = false;
	bool loading = false;
	bool loadingPath = false;
	bool goingBack = false;

	std::map<int, NodeEntity*> mapSimToGraphNodes;
	std::map<int, LinkEntity*> mapSimToGraphLinks;
	std::map<int, EmptyEntity*> mapSimToGraphPaths;

	void setPathLoading(bool loading)
	{
		loadingPath = loading;
	}

	bool isSaving() {
		return saving;
	}

	void setNewMap(bool m_startingNew)
	{
		startingNew = m_startingNew;
	}

	void setLoading(bool m_loading)
	{
		loading = m_loading;
	}

	bool isStartingNew()
	{
		return startingNew;
	}

	bool isLoading()
	{
		return loading;
	}

	bool isLoadingPath()
	{
		return loadingPath;
	}

	bool isGoingBack()
	{
		return goingBack;
	}

	std::string getPathLoading() {
		return pathLoading;
	}

	void SetGoingBack(bool m_goingBack) {
		goingBack = m_goingBack;
	}

	void updateFileNamesInAssets() {
		fileNames.clear();
		const std::string path = "assets/Maps"; // Directory path
		for (const auto& entry : fs::directory_iterator(path)) {
			if (entry.is_regular_file()) {
				fileNames.push_back(entry.path().filename().string()); // Add file name to vector
			}
		}
	}

	void updatePathFileNamesInAssets() {
		pathsFileNames.clear();
		const std::string path = "assets/Paths"; // Directory path
		for (const auto& entry : fs::directory_iterator(path)) {
			if (entry.is_regular_file()) {
				pathsFileNames.push_back(entry.path().filename().string()); // Add file name to vector
			}
		}
		pathsFileNames.push_back(">Reset");
	}

	void ReloadAccessibleFiles() {
		if (!filesLoaded) {
			updateFileNamesInAssets();
			updatePathFileNamesInAssets();

			filesLoaded = true; // Set to true so we don't reload unnecessarily
		}
	}

	void applyStep(Manager& manager, int transitionToStep)
	{
		// Nodes

		auto stepIt = manager.steps.begin();
		std::advance(stepIt, transitionToStep);

		for (auto& node : stepIt->nodes) {
			NodeEntity* t_node = node.first;

			if (t_node->hasComponent<TransformComponent>() &&
				//t_node->hasComponent<MovingAnimatorComponent>() &&
				t_node->hasComponent<RectangleFlashAnimatorComponent>()
				) {

				auto& tc = t_node->GetComponent<TransformComponent>();

				/*t_node->GetComponent<MovingAnimatorComponent>().Play("StepMove",*/
				tc.position = node.second.position;

				tc.size = glm::vec3(node.second.size);

				t_node->GetComponent<RectangleFlashAnimatorComponent>().Play("RectInterpolation", node.second.color);

			}
		}

		// Links
		for (auto& link : stepIt->links) {
			LinkEntity* t_link = link.first;

			if (t_link->hasComponent<Line_w_Color>()) {
				auto& lwc = t_link->GetComponent<Line_w_Color>();
				lwc.src_color = link.second.color;
				lwc.dest_color = link.second.color;
				lwc.width = link.second.width;
			}
		}

		for (auto& simPaths : DataManager::getInstance().mapSimToGraphPaths) {
			auto& pathLinks = simPaths.second->GetComponent<PathLinkerComponent>().pathLinks;
			auto toRemove = pathLinks;

			for (auto pathLinkId : toRemove) {
				simPaths.second->GetComponent<PathLinkerComponent>().removeLink(pathLinkId);
				Entity* pathLink = manager.getEntityFromId(pathLinkId);
				pathLink->destroy();
			}
		}

		// Paths
		for (size_t i = 0; i < stepIt->paths.size(); i++) {

			auto stepIt = manager.steps.begin();
			std::advance(stepIt, transitionToStep);

			auto& path = stepIt->paths[i];

			auto& plc = path.first->GetComponent<PathLinkerComponent>();

			plc.color = path.second.color;
			plc.width = path.second.width;

			for (auto linkId : path.second.link_ids) {
				// Find the link entity by ID
				LinkEntity* linkEntity = nullptr;
				for (auto& link : DataManager::getInstance().mapSimToGraphLinks) {
					if (link.second->getId() == linkId) {
						linkEntity = link.second;
						break;
					}
				}

				if (!linkEntity) break;

				int idA = std::get<int>(linkEntity->getFromNode());
				int idB = std::get<int>(linkEntity->getToNode());

				// create ECS link
				auto& link = manager.addEntity<Link>((int)idA, (int)idB);
				link.addToGroup(Manager::groupPathLinks);
				link.addComponent<Line_w_Color>();

				link.addComponent<LineFlashAnimatorComponent>();

				manager.grid->addLink(&link, manager.grid->getGridLevel());

				// associate link with path linker
				path.first->GetComponent<PathLinkerComponent>().addLink(link.getId());

			}
		}

		// PATH LINKS UPDATE
		if (manager.arrowheadsEnabled) {
			for (auto* link : manager.getGroup<LinkEntity>(Manager::groupPathLinks))
			{
				auto* link_entity = dynamic_cast<LinkEntity*>(link);

				link_entity->setConnectionType(LinkEntity::ConnectionType::PORT_TO_PORT);
				link_entity->updateConnection();
			}

			for (auto* pathLinker : manager.getGroup<EmptyEntity>(Manager::groupPathLinksHolder))
			{
				pathLinker->GetComponent<PathLinkerComponent>().createInnerLinks();
			}
		}
	}

	void copySimulationStepTo(
		Manager& manager,
		sim_dump::UInt32 sourceStepIndex,
		sim_dump::UInt32 targetStepIndex,
		double targetTimestamp
	)
	{
		auto src = std::find_if(
			manager.steps.begin(),
			manager.steps.end(),
			[&](const SimulationStep& s) {
				return s.step_index == sourceStepIndex;
			}
		);

		if (src == manager.steps.end()) {
			TAZ_ERROR("Source SimulationStep not found");
			return;
		}

		SimulationStep copied = *src; // deep copy of vectors
		copied.step_index = targetStepIndex;
		copied.timestamp = targetTimestamp;

		manager.steps.push_back(std::move(copied));
	}


	void reindexSteps(Manager& manager)
	{
		sim_dump::UInt32 idx = 0;
		for (auto& step : manager.steps) {
			step.step_index = idx++;
		}
	}

	void addSimulationStep(Manager& manager) {
		SimulationStep new_step = SimulationStep();

		new_step.step_index = manager.steps.back().step_index + 1;
		new_step.timestamp = manager.steps.back().timestamp + 0.01f;

		manager.steps.push_back(new_step);

		reindexSteps(manager);
	}


	void addSimulationStep(Manager& manager, sim_dump::UInt32 step, double timestamp, sim_dump::UInt32 copyStep) {
		SimulationStep new_step = SimulationStep();

		new_step.step_index = step;
		new_step.timestamp = timestamp;

		if (step >= manager.steps.size()) {
			manager.steps.push_back(new_step);
			return;
		}

		auto it = manager.steps.begin();
		std::advance(it, step);

		manager.steps.insert(it, new_step); // there are 2 steps with same index but 
		//newly insterted is before the other

		reindexSteps(manager);
	}

	void removeSimulationStep(Manager& manager, sim_dump::UInt32 step) {
		if (step >= manager.steps.size()) {
			TAZ_ERROR("SimulationStep to remove not found");
			return;
		}
		auto it = manager.steps.begin();
		std::advance(it, step);

		manager.steps.erase(it);

		reindexSteps(manager);
	}


	std::string simulationStepToString(const SimulationStep& step)
	{
		std::ostringstream out;

		out << "=== Simulation Step ===\n";
		out << "Index: " << step.step_index << "\n";
		out << "Timestamp: " << step.timestamp << "\n";

		// ---- Nodes ----
		out << "Nodes (" << step.nodes.size() << ")\n";
		for (const auto& [node, simNode] : step.nodes) {
			out << "  Node ptr: " << node
				<< " pos: (" << simNode.position.x
				<< ", " << simNode.position.y
				<< ", " << simNode.position.z << ")"
				<< " size: " << simNode.size.value
				<< "\n";
		}

		// ---- Links ----
		out << "Links (" << step.links.size() << ")\n";
		for (const auto& [link, simLink] : step.links) {
			out << "  Link ptr: " << link
				<< " width: " << simLink.width
				<< "\n";
		}

		// ---- Paths ----
		out << "Paths (" << step.paths.size() << ")\n";
		for (const auto& [pathEntity, simPath] : step.paths) {
			out << "  Path ptr: " << pathEntity
				<< " width: " << simPath.width
				<< " links: ";

			for (EntityID id : simPath.link_ids) {
				out << EntityIDUtils::toString(id) << " ";
			}
			out << "\n";
		}

		out << "=======================\n";

		return out.str();
	}

	std::string simulationStepToString(const Manager& manager, int stepIndex)
	{
		std::ostringstream out;

		auto it = std::find_if(
			manager.steps.begin(),
			manager.steps.end(),
			[&](const SimulationStep& s) { return s.step_index == stepIndex; }
		);

		if (it != manager.steps.end()) {
			out << simulationStepToString(*it);
		}
		else {
			out << "SimulationStep " << stepIndex << " not found.\n";
		}

		return out.str();
	}
};