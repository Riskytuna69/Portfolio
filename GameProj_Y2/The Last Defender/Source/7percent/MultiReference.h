#pragma once
#include "EntityUID.h"

// Stuff has gotten annoying enough that I'm making this component.
class MultiReferenceComponent : public IRegisteredComponent<MultiReferenceComponent>
#ifdef IMGUI_ENABLED
	, IEditorComponent<MultiReferenceComponent>
#endif
	, public ecs::IComponentCallbacks
{
public:
	MultiReferenceComponent();
	std::vector<EntityReference>& GetReferences();
	void SetReferences(std::vector<EntityReference> const& vec);
private:
	int numReferences;
	std::vector<EntityReference> vecReferences;
#ifdef IMGUI_ENABLED
	/*****************************************************************//*!
	\brief
		Editor draw function, draws the IMGui elements to allow the
		component's values to be edited. Disabled when IMGui is disabled.
	\param comp
		The component instance.
	*//******************************************************************/
	static void EditorDraw(MultiReferenceComponent& comp);
#endif
	property_vtable()
};
property_begin(MultiReferenceComponent)
{
	property_var(vecReferences)
}
property_vend_h(MultiReferenceComponent)