#include "VisibleEntities.h"

void VisibleEntitiesPanel::OnImGuiRender()
{
	ImGui::Begin("VisibleEntities", &showVisibleEntities);

	if (ImGui::BeginTable("GroupsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("Group Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Entity Count", ImGuiTableColumnFlags_WidthFixed, 100.0f);
		ImGui::TableHeadersRow();

		int totalEntities = 0;
		for (std::size_t managerGroup = Manager::groupBackgroundLayer; managerGroup != Manager::buttonLabels + 1; ++managerGroup) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", config.manager->getGroupName(managerGroup).c_str());
			ImGui::TableSetColumnIndex(1);
			int groupSize = config.manager->getVisibleGroup<EmptyEntity>(managerGroup).size() + config.manager->getVisibleGroup<NodeEntity>(managerGroup).size() + config.manager->getVisibleGroup<LinkEntity>(managerGroup).size();
			ImGui::Text("%d", groupSize);

			totalEntities += groupSize;
		}

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Total Visible Entities");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%d", totalEntities);

		ImGui::EndTable();
	}

	if (ImGui::BeginTable("GroupsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("Vectors", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Size()", ImGuiTableColumnFlags_WidthFixed, 100.0f);
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("entities");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", config.manager->getEntities().size());

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("visible empty entities");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", config.manager->getVisible<EmptyEntity>().size());

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("visible nodes");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", config.manager->getVisible<NodeEntity>().size());

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("visible links");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("%zu", config.manager->getVisible<LinkEntity>().size());

		ImGui::EndTable();
	}

	ImGui::End();
}
