#include "pch.h"
#include "SplashComponent.h"
#include "SceneTransitionManager.h"
#include "SceneManagement.h"

SplashComponent::SplashComponent()
	: scheduledSceneSwitch{ false }
{
}

bool SplashComponent::GetHasScheduled() const
{
	return scheduledSceneSwitch;
}

void SplashComponent::MarkAsScheduled()
{
	scheduledSceneSwitch = true;
}

SplashSystem::SplashSystem()
	: System_Internal{ &SplashSystem::UpdateComp }
{
}

void SplashSystem::UpdateComp(SplashComponent& comp)
{
	if (comp.GetHasScheduled())
		return;
	comp.MarkAsScheduled();

	ST<Scheduler>::Get()->Add(3.25f, []() -> void {
		ST<SceneTransitionManager>::Get()->TransitionScene("SplashScene", "MenuScene");
	})/*.Then(1.5f, []() -> void {
		ST<SceneManager>::Get()->LoadScene(ST<Filepaths>::Get()->scenesSave + "/ParallaxScene.scene", false);
	})*/;
}
