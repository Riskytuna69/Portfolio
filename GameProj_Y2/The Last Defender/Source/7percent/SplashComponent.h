#pragma once

class SplashComponent
	: public IRegisteredComponent<SplashComponent>
{
public:
	SplashComponent();

	bool GetHasScheduled() const;
	void MarkAsScheduled();

private:
	bool scheduledSceneSwitch;

public:
	property_vtable()
};
property_begin(SplashComponent)
{
}
property_vend_h(SplashComponent)

class SplashSystem : public ecs::System<SplashSystem, SplashComponent>
{
public:
	SplashSystem();

private:
	void UpdateComp(SplashComponent& comp);

};
