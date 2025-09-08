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


class UIElement {
public:
    UIElement() = default;
    virtual ~UIElement() = default;
    virtual void update(float deltaTime = 0.0f) {} //no need to implement
    virtual void OnImGuiRender() = 0; //must implement

protected:
    std::string name;
    bool visible = true;
};
