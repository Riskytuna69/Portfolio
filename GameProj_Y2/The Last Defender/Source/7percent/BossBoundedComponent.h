#pragma once
class BossBoundedComponent : public IRegisteredComponent<BossBoundedComponent>
{
public:
	/*****************************************************************//*!
	\brief
		Default constructor.
	\return
	*//******************************************************************/
	BossBoundedComponent();

	/*****************************************************************//*!
	\brief
		Default destructor.
	\return
	*//******************************************************************/
	~BossBoundedComponent();

	property_vtable()
};
property_begin(BossBoundedComponent)
{

}
property_vend_h(BossBoundedComponent)

