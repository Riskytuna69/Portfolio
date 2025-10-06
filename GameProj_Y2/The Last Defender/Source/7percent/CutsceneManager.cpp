/******************************************************************************/
/*!
\file   CutsceneManager.cpp
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   04/02/2025

\author Marc Alviz Evangelista (100%)
\par    email: marcalviz.e\@digipen.edu
\par    DigiPen login: marcalviz.e

\brief

	Component that manages the cutscene


All content ? 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "CutsceneManager.h"
#include "GameTime.h"
#include "AnimatorComponent.h"
#include "NameComponent.h"
#include "TweenManager.h"
#include "SceneTransitionManager.h"
#include "CameraController.h"
#include "AudioManager.h"
#include "SceneManagement.h"

CutsceneManagerComponent::CutsceneManagerComponent() :
#ifdef IMGUI_ENABLED
	REGISTER_DRAW_FUNCTION_TO_EDITOR(EditorDraw),
#endif
	fadeTimer { 0.0f },
	fadeTime { 2.0f },
	loopingTimer { 0.0f },
	loopingTime { 2.0f },
	anim02Done { false },
	transitionStarted { false },
	anim01{ nullptr },
	anim02{ nullptr },
	anim03{ nullptr },
	anim04{ nullptr },
	fadeEntity{ nullptr },
	skipCutsceneText{ nullptr },
	currAnimPlaying{ nullptr },
	currAnimScene { FADEIN01 },
	isInit { false }
{
}

CutsceneManagerComponent::~CutsceneManagerComponent()
{
}
void CutsceneManagerComponent::Init()
{
	isInit = true;

	fadeTimer = 0.0f;
	loopingTimer = 0.0f;
	transitionStarted = false;
	for (auto ite = ecs::GetCompsBegin<RenderComponent>(); ite != ecs::GetCompsEnd<RenderComponent>(); ++ite)
	{
		NameComponent* name = ite.GetEntity()->GetComp<NameComponent>();
		if (name->GetName() == "Anim01_CompOn")
		{		
			anim01 = ite.GetEntity();
		}
		else if (name->GetName() == "Anim02_ProtocolLoop")
		{
			anim02 = ite.GetEntity();
		}
		else if (name->GetName() == "Anim03_VisorOn")
		{
			anim03 = ite.GetEntity();
		}
		else if (name->GetName() == "Anim04_VisorLoop")
		{
			anim04 = ite.GetEntity();
		}
	}

	anim01->GetComp<AnimatorComponent>()->Reset();
	anim01->GetComp<AnimatorComponent>()->Pause();
	anim02->GetComp<AnimatorComponent>()->Reset();
	anim02->GetComp<AnimatorComponent>()->Pause();
	anim03->GetComp<AnimatorComponent>()->Reset();
	anim03->GetComp<AnimatorComponent>()->Pause();
	anim04->GetComp<AnimatorComponent>()->Reset();
	anim04->GetComp<AnimatorComponent>()->Pause();

	anim01->SetActive(true);
	anim02->SetActive(false);
	anim03->SetActive(false);
	anim04->SetActive(false);

	anim01->GetComp<AnimatorComponent>()->SetLooping(false);
	anim02->GetComp<AnimatorComponent>()->SetLooping(true);
	anim03->GetComp<AnimatorComponent>()->SetLooping(false);
	anim04->GetComp<AnimatorComponent>()->SetLooping(true);


	currAnimScene = FADEIN01;
	currAnimPlaying = anim01;
	anim02Done = false;

	ST<AudioManager>::Get()->SetChannelGroup("Cutscene_Ambience", "BGM");
	ST<AudioManager>::Get()->SetChannelGroup("Cutscene_Static", "BGM");

	ST<AudioManager>::Get()->StartSingleSound("Cutscene_Ambience", true);
	ST<AudioManager>::Get()->StartSingleSound("Cutscene_Static", true);
}
bool CutsceneManagerComponent::GetIsInit() const
{
	return isInit;
}
void CutsceneManagerComponent::Update()
{
	// Only able to start a new transition once previous transition has ended
	if (!ST<SceneManager>::Get()->GetSceneWithName("TransitionScene"))
	{
		skipCutsceneText->SetActive(true);

		if (Input::GetKeyPressed(KEY::ESC))
		{
			currAnimScene = TRANSITION;
		}
	}
	else
	{
		skipCutsceneText->SetActive(false);
	}

	// Different behaviours for different animations
	switch (currAnimScene)
	{
	case ANIM01:
	{
		if (currAnimPlaying->GetComp<AnimatorComponent>()->IsPlaying())
			break;
		currAnimPlaying->SetActive(false);
		if (currAnimScene == ANIM01)
		{
			currAnimPlaying = anim02;
		}
		else
		{
			currAnimPlaying = anim04;
		}
		currAnimPlaying->SetActive(true);
		currAnimPlaying->GetComp<AnimatorComponent>()->Play();
		currAnimScene = static_cast<ANIM_SCENE>(static_cast<int>(currAnimScene) + 1);

		ecs::CompHandle<RenderComponent> renderComp = fadeEntity->GetComp<RenderComponent>();
		Vector4 initialColor = renderComp->GetColor(), finalColor = initialColor;
		finalColor.w = 1.0f;
		ST<TweenManager>::Get()->StartTween(
			fadeEntity,
			&RenderComponent::SetColor,
			initialColor,
			finalColor,
			fadeTime,
			TT::EASE_OUT);
		break;
	}
	case ANIM03:
	{
		if (currAnimPlaying->GetComp<AnimatorComponent>()->IsPlaying())
			break;
		currAnimPlaying->SetActive(false);
		if (currAnimScene == ANIM01)
		{
			currAnimPlaying = anim02;
		}
		else
		{
			currAnimPlaying = anim04;
		}
		currAnimPlaying->SetActive(true);
		currAnimPlaying->GetComp<AnimatorComponent>()->Play();
		ST<AudioManager>::Get()->StartSingleSound("Cutscene_Visor_On");
		currAnimScene = static_cast<ANIM_SCENE>(static_cast<int>(currAnimScene) + 1);
		break;
	}
	case ANIM02:
	case ANIM04:
	{
		// for the looping animations
		// loops for awhile then fades black while looping
		loopingTimer += GameTime::FixedDt();
		if (loopingTimer >= loopingTime)
		{
			loopingTimer = 0.0f;
			if (currAnimScene == ANIM02)
			{
				anim02Done = true;
				fadeTimer = fadeTime * 2.0f;
			}

			currAnimScene = static_cast<ANIM_SCENE>(static_cast<int>(currAnimScene) + 1);
		}
		break;
	}
	case FADEIN01: // Fading from black into animation
	case FADEIN02:
	{
		if (anim02Done)
		{
			// hit once only
			anim02Done = false;
			currAnimPlaying->SetActive(false);
			currAnimPlaying = anim03;
			currAnimPlaying->SetActive(true);
		}
		fadeTimer += GameTime::FixedDt();
		if (fadeTimer >= fadeTime)
		{
			currAnimScene = static_cast<ANIM_SCENE>(static_cast<int>(currAnimScene) + 1);
			fadeTimer = 0.0f;
			currAnimPlaying->GetComp<AnimatorComponent>()->Play();
			
		}
		break;
	}
	case FADEOUT01: // Fading out to black -> happens while the looping animations play
	case FADEOUT02:
	{
		fadeTimer += GameTime::FixedDt();
		if (fadeTimer >= fadeTime)
		{
			fadeTimer = 0.0f;
			currAnimScene = static_cast<ANIM_SCENE>(static_cast<int>(currAnimScene) + 1);
			ecs::CompHandle<RenderComponent> renderComp = fadeEntity->GetComp<RenderComponent>();
			Vector4 initialColor = renderComp->GetColor(), finalColor = initialColor;
			finalColor.w = 0.0f;
			ST<TweenManager>::Get()->StartTween(
				fadeEntity,
				&RenderComponent::SetColor,
				initialColor,
				finalColor,
				fadeTime,
				TT::EASE_OUT);
		}
		break;
	}
	case TRANSITION:
	{
		// go to next scene here
		if (!transitionStarted)
		{
			transitionStarted = true;
			ST<SceneTransitionManager>::Get()->TransitionScene("Cutscene Scene", "NEW_LEVEL");
			currAnimScene = static_cast<ANIM_SCENE>(static_cast<int>(currAnimScene) + 1);
		}
		break;
	}
	default:
		break;
	}
}
#ifdef IMGUI_ENABLED
void CutsceneManagerComponent::EditorDraw(CutsceneManagerComponent& comp)
{
	ImGui::PushID("CSFADETIME");
	ImGui::Text("Fade Time");
	ImGui::InputFloat("", &comp.fadeTime);
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("ANIM01");
	ImGui::Text("Anim_01");
	comp.anim01.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("ANIM02");
	ImGui::Text("Anim_02");
	comp.anim02.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("ANIM03");
	ImGui::Text("Anim_03");
	comp.anim03.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("ANIM04");
	ImGui::Text("Anim_04");
	comp.anim04.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("FADEENTITY");
	ImGui::Text("Fade Entity");
	comp.fadeEntity.EditorDraw("");
	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("SKIPTEXT");
	ImGui::Text("Skip Text");
	comp.skipCutsceneText.EditorDraw("");
	ImGui::PopID();

	ImGui::Text("~~~~~~~~~~~~~~DEBUG~~~~~~~~~~~~~~");

	ImGui::NewLine();

	ImGui::PushID("FADETIMER");
	ImGui::Text("Fade Timer");
	ImGui::InputFloat("", &comp.fadeTimer);

	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("LOOPTIMER");
	ImGui::Text("Loop Timer");
	ImGui::InputFloat("", &comp.loopingTimer);

	ImGui::PopID();

	ImGui::NewLine();

	ImGui::PushID("CURRENTANIMATION");
	ImGui::Text("Current Anim");
	comp.currAnimPlaying.EditorDraw("");
	ImGui::PopID();


	ImGui::NewLine();

	ImGui::PushID("CURRENTANIMSCENE");
	ImGui::Text("Current Anim Scene: ");
	ImGui::Text(std::to_string(comp.currAnimScene).c_str());
	ImGui::PopID();


}
#endif

CutsceneSystem::CutsceneSystem()
	: System_Internal{ &CutsceneSystem::Update }
{
}

void CutsceneSystem::Update(CutsceneManagerComponent& comp)
{
	if (!comp.GetIsInit())
	{
		comp.Init();
	}

	comp.Update();
}
