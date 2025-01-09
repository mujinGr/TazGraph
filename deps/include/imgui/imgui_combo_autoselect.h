#pragma once

#include <cstring>
#include <vector>
#include <string>
#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{
	struct ComboAutoSelectData {
		std::vector<std::string> items;
		int index = -1;
		char input[128] = {};

		ComboAutoSelectData()
		{
			
		}

		void SetSelectData(std::vector<std::string>&& hints, int selected_index = -1) {
			items = hints;

			if (selected_index > -1 && selected_index < (int)items.size()) {
				const std::string& selected_item = items[selected_index];
        		size_t input_len = sizeof(input); 
				
				 strncpy(input, selected_item.c_str(), input_len - 1);

				// Ensure null termination
				input[input_len - 1] = '\0';
				
				//strncpy_s(input, sizeof(input), items[selected_index].c_str(), sizeof(input) - 1);
				index = selected_index;
			}
		}
	};

	bool ComboAutoSelect(const char* label, ComboAutoSelectData & data, ImGuiComboFlags flags = 0);

}