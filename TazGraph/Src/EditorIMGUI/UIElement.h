#pragma once

#include "TazGraphEngine.h"

#include "../AssetManager/AssetManager.h"

struct SelectedInfo {
	EntityID realEntityId;     // the actual Node / Link selected
	EntityID overlayEntityId;  // the entity used for rendering selection box
	glm::vec3 relativeOffset;
};

std::vector<EntityID> selectedEntities_RealIds(std::vector<SelectedInfo> sel_entities);


class UIElement {
public:
	inline static std::unordered_map<std::type_index, UIElement*> uiComponentRegistry;

	std::vector<std::unique_ptr<UIElement>> subcomponents;
	UIElement* parent = nullptr; // Pointer to parent element

	UIElement() {
		// Auto-register on construction
		registerUIComponent(this);
	};
	virtual ~UIElement() = default;

	virtual void update(float deltaTime = 0.0f) {
		for (auto& component : subcomponents) {
			component->update(deltaTime);
		}
	}

	virtual void OnImGuiRender() = 0; // must implement

	template<typename T, typename... Args>
	void addUIComponent(Args&&... args) {
		auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);
		newComponent->parent = this; // Set parent pointer
		subcomponents.push_back(std::move(newComponent));
	}

	template<typename T>
	static T* getUIComponent() {
		auto it = uiComponentRegistry.find(std::type_index(typeid(T)));
		if (it != uiComponentRegistry.end()) {
			return dynamic_cast<T*>(it->second);
		}
		return nullptr;
	}

	template<typename T>
	T* getSubcomponent() {
		for (auto& component : subcomponents) {
			if (T* casted = dynamic_cast<T*>(component.get())) {
				return casted;
			}
		}
		return nullptr;
	}

protected:
	std::string name;
	bool visible = true;

private:
	// Helper methods for registration
	static void registerUIComponent(UIElement* component) {
		uiComponentRegistry[std::type_index(typeid(*component))] = component;
	}

};
