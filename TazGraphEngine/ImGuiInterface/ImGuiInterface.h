#pragma once

#include <iostream>
#include <string>

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <implot_internal.h>
#include "../imguiComboAutoselect/imgui_combo_autoselect.h"

class ImGuiInterface {
public:
    ImGuiInterface();

    ~ImGuiInterface();

    //void SetupImGui();

    void BeginRender();

    void RenderUI();

    void EndRender();
};