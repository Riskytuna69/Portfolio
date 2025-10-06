/******************************************************************************/
/*!
\file   HitFlash.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   09/03/2025

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  Hitflash effect which is called (and should only be called) by HealthComponent.
  As such, ensure HitFlash component is on the same entity as HealthComponent.
  It only cares about render components within children.

All content © 2025 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "HitFlash.h"
#include "RenderComponent.h"

HitFlashComponent::HitFlashComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	flashMaterialName{ "HitFlash" },
	vecRenderComps{},
	inited{ false },
	timer{ 0.0f },
	duration{ 0.0f },
	defaultMaterialNames{}
{
}

void HitFlashComponent::FlashForSeconds(float seconds)
{
	duration = seconds;
	timer = 0.0f;

	// Iterate
	for (int i = 0; i < vecRenderComps.size(); ++i)
	{
		if (!vecRenderComps[i])
		{
			continue;
		}
		vecRenderComps[i]->GetComp<RenderComponent>()->SetMaterial(flashMaterialName);
	}
}

void HitFlashComponent::ReturnToOriginalMaterial()
{
	// Iterate
	for (int i = 0; i < vecRenderComps.size(); ++i)
	{
		if (!vecRenderComps[i])
		{
			continue;
		}
		if (auto renderComp{ vecRenderComps[i]->GetComp<RenderComponent>() })
			renderComp->SetMaterial(defaultMaterialNames[i]);
	}
}

void HitFlashComponent::Init()
{
	inited = true;

	// Iterate render comps
	StoreRenderComps(ecs::GetEntityTransform(this).GetChildren());
}

void HitFlashComponent::StoreRenderComps(std::set<Transform*> const& children)
{
	for (auto child : children)
	{
		ecs::EntityHandle entity = child->GetEntity();
		ecs::CompHandle<HitFlashComponent> hitFlashComp = entity->GetComp<HitFlashComponent>();

		// If this child already has a hit flash component, skip
		if (hitFlashComp)
		{
			continue;
		}

		// Get render component
		ecs::CompHandle<RenderComponent> renderComp = entity->GetComp<RenderComponent>();

		// If it has a render comp, add it to the vector
		if (renderComp)
		{
			// Store reference
			vecRenderComps.push_back(entity);

			// Store material name
			defaultMaterialNames.push_back(renderComp->GetMaterialName());
		}

		// Also explore children of this child
		StoreRenderComps(child->GetChildren());
	}
}

void HitFlashComponent::OnDetached()
{
	ReturnToOriginalMaterial();
}

#ifdef IMGUI_ENABLED
void HitFlashComponent::EditorDraw(HitFlashComponent& comp)
{
	static char buffer[256];
	strncpy_s(buffer, comp.flashMaterialName.c_str(), sizeof(buffer) - 1);
	if (ImGui::InputText("Flash Material", buffer, sizeof(buffer)))
	{
		comp.flashMaterialName = buffer;
	};
}
#endif

HitFlashSystem::HitFlashSystem()
	: System_Internal{ &HitFlashSystem::UpdateHitFlashComponent }
{
}

void HitFlashSystem::UpdateHitFlashComponent(HitFlashComponent& comp)
{
	if (!comp.inited)
	{
		comp.Init();
	}

	// The way we tell if HitFlash should be updated is whether the duration is +ve.
	if (comp.duration < 0.0f) return;

	// If duration is +ve and timer is lesser, increment
	if (comp.timer < comp.duration)
	{
		comp.timer += GameTime::FixedDt();
	}
	else // Else the timer has reached duration and we should reset
	{
		comp.timer = 0.0f;
		comp.duration = -1.0f;
		comp.ReturnToOriginalMaterial();
	}
}
