#pragma once
#include <string>
#include <imgui.h>
#include <implot/implot.h>
#include <glm/glm.hpp> 
#include <vector>
#include <utility> 

class UIElement {
public:
    UIElement() = default;
    virtual ~UIElement() = default;
    virtual void update() {} //no need to implement
    virtual void OnImGuiRender() = 0; //must implement

protected:
    std::string name;
    bool visible = true;
};
