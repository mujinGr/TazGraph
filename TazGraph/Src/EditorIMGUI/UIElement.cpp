
#include "UIElement.h"

std::vector<EntityID> selectedEntities_RealIds(std::vector<SelectedInfo> sel_entities) {
	std::vector<EntityID> result;

	for (auto sel_ent : sel_entities) {
		result.push_back(sel_ent.realEntityId);
	}

	return result;
}