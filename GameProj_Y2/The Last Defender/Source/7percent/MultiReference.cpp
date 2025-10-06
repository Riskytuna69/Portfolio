#include "MultiReference.h"

MultiReferenceComponent::MultiReferenceComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	numReferences{ 1 },
	vecReferences{ std::vector<EntityReference>(numReferences, nullptr) }
{
}

std::vector<EntityReference>& MultiReferenceComponent::GetReferences()
{
	return vecReferences;
}

void MultiReferenceComponent::SetReferences(std::vector<EntityReference> const& vec)
{
    vecReferences = vec;
}

#ifdef IMGUI_ENABLED
void MultiReferenceComponent::EditorDraw(MultiReferenceComponent& comp)
{
    // Input field to modify numReferences
    ImGui::InputInt("Number of References", &comp.numReferences);

    // Clamp to 1
    if (comp.numReferences < 1)
    {
        comp.numReferences = 1;
    }

    // Add button to apply changes
    if (ImGui::Button("Resize"))
    {
        comp.vecReferences.resize(static_cast<size_t>(comp.numReferences));
    }

    // Draw input fields for EntityReferences
    int i = 0;
    for (EntityReference& ref : comp.vecReferences)
    {
        ref.EditorDraw(std::to_string(i++).c_str());
    }
}
#endif