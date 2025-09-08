#pragma once
#include <string>
#include <imgui.h>
#include <implot/implot.h>
#include <glm/glm.hpp> 
#include <ImGuizmo/ImGuizmo.h>
#include <ImGuiComboAutoselect/imgui_combo_autoselect.h>
#include <vector>
#include <utility> 

#include <BaseFPSLimiter/BaseFPSLimiter.h>
#include <DataManager/DataManager.h>

#include "GECS/Components.h"
#include "GECS/UtilComponents.h"
#include "GECS/Core/GECSManager.h"

#include <GECS/Core/GECSUtil.h>
#include <typeindex>


class UIElement {
public:
    static std::unordered_map<std::type_index, UIElement*> uiComponentRegistry;

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
