#pragma once

#include <string>
#include <TextureManager/TextureManager.h>
#include <GECS/Core/GECSEntityTypes/GECSEntityTypes.h>
#include <ImGuiInterface/ImGuiInterface.h>

#include <SDL2/SDL_ttf.h>
#include <ImGuiText.h>

#include <GECS/UtilComponents.h>

struct PairHash {
	template <class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2>& p) const {
		auto hash1 = std::hash<T1>{}(p.first);
		auto hash2 = std::hash<T2>{}(p.second);
		return hash1 ^ (hash2 << 1);
	}
};
namespace AssetManager //this class created when we added projectiles, based on this class other components changed
{					//it just replaces the paths of textures with names

	//graphobjects
	void CreateWorldMap(Entity& worldMap);
	void CreateGroup(Entity& groupNode, glm::vec3 centerGroup, float groupNodeSize, Grid::Level m_level);

	void CreateGroupLink(Entity& groupLink, Grid::Level m_level);

	void createGroupLayout(Manager* manager, Grid::Level m_level);

	void ungroupLayout(Manager* manager, Grid::Level m_level);
	void AddDefaultNode(Entity& node, glm::vec3 mPosition);
	void AddTreeNode(Entity& node, glm::vec3 mPosition);
	void AddDefaultLink(Entity& link);
	void AddTreeLink(Entity& link);
	void AddPathLink(Entity& link);
	void AddSimulationNode(Entity& node, glm::vec3 mPosition);
	void AddSimulationLink(Entity& link);
};