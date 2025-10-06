#pragma once
#include "EntityUID.h"

class ItemInfoPanelComponent : public IRegisteredComponent<ItemInfoPanelComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<ItemInfoPanelComponent>
#endif
{
public:
	EntityReference entityName;
	EntityReference entityCurrent;
	EntityReference entityMaximum;

	ItemInfoPanelComponent();
	void UpdateItemInfo(EntityReference& item);
private:
#ifdef IMGUI_ENABLED
	static void EditorDraw(ItemInfoPanelComponent& comp);
#endif
	property_vtable()
};
property_begin(ItemInfoPanelComponent)
{
	property_var(entityName),
	property_var(entityCurrent),
	property_var(entityMaximum)
}
property_vend_h(ItemInfoPanelComponent)
