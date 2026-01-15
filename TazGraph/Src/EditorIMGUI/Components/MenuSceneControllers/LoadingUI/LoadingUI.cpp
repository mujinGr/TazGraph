#include "LoadingUI.h"

static ImGui::FileBrowser fileDialog;

void LoadingUI::OnImGuiRender()
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize(400, 100); // Desired window size
	ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f,
		(io.DisplaySize.y - windowSize.y) * 0.5f);

	ImGui::SetNextWindowPos(windowPos);
	ImGui::SetNextWindowSize(windowSize);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;
	bool open = true;

	if (!ImGui::IsPopupOpen("Loading...")) {
		ImGui::OpenPopup("Loading...");
	}


	ImGui::BeginPopupModal("Loading...", &open, window_flags);

	DataManager::getInstance().ReloadAccessibleFiles();

	DataManager::getInstance().data.SetSelectData(std::move(DataManager::getInstance().fileNames));

	if (ImGui::ComboAutoSelect("Select File", DataManager::getInstance().data)) {
	}

	ImGui::SameLine();
	if (ImGui::Button("Browse...", ImVec2(100, 0))) {
		fileDialog.SetTitle("Select Data File");
		fileDialog.SetTypeFilters({ ".json", ".csv", ".txt", ".cpp", ".h", ".simdmp", ".dot", ".graphml" }); // Add your supported file types
		fileDialog.Open();
	}

	ImGui::NewLine();
	float windowWidth = ImGui::GetContentRegionAvail().x;
	float buttonWidth = 100; // Define the button width you want
	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f); // Center the button

	if (ImGui::Button("Load", ImVec2(buttonWidth, 0))) {
		DataManager::getInstance().loading = false;
	}

	if (!open) {
		std::memset(DataManager::getInstance().data.input, 0, sizeof(DataManager::getInstance().data.input));
		DataManager::getInstance().loading = false;
	}

	// Render the file browser window
	fileDialog.Display();

	// Handle file selection
	if (fileDialog.HasSelected()) {
		std::string selectedPath = fileDialog.GetSelected().string();
		fileDialog.ClearSelected();

		// Reference to your combo data
		auto& comboData = DataManager::getInstance().data;

		// If the selected file isn’t already in the combo list, add it
		auto it = std::find(comboData.items.begin(), comboData.items.end(), selectedPath);
		if (it == comboData.items.end()) {
			comboData.items.push_back(selectedPath);
			comboData.index = static_cast<int>(comboData.items.size() - 1);
		}
		else {
			comboData.index = static_cast<int>(std::distance(comboData.items.begin(), it));
		}

		// Copy file name into the combo input buffer
#ifdef _WIN32
		strncpy_s(comboData.input, sizeof(comboData.input), selectedPath.c_str(), _TRUNCATE);
#else
		strncpy(comboData.input, selectedPath.c_str(), sizeof(comboData.input) - 1);
		comboData.input[sizeof(comboData.input) - 1] = '\0';
#endif

		// Store the full path somewhere if you use it for loading
		DataManager::getInstance().mapToLoad = selectedPath;

		// Optional: automatically close loading UI
		DataManager::getInstance().loading = false;
	}

	ImGui::EndPopup();
}
