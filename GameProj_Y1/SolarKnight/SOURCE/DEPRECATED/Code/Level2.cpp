/* Start Header ************************************************************************/
/*
* LEVEL 2
*/
/* End Header **************************************************************************/
#include "pch.h"

#include "Level2.h"
#include "GameStateManager.h"
#include "DayNight.h"

namespace
{
	typedef struct Color
	{
		float r;
		float g;
		float b;
		float a;
	}Color;
	Color ColorTransition(const Color& start, const Color& end, float time);
}

namespace Level2 {
	s8 pFont;
	f32 windowWidth;
	f32 windowHeight;

	// Day / Night
	DayNightCycle::DayNightState dnState;
	DayNightCycle::DayNightState dnPrevState;
	bool bSkyColorTransiting;
	f32 dayTime;
	f32 eveningTime;
	f32 nightTime;
	f32 skyTranisitionTime;
	Color skyColor;
	Color cyan;
	Color black;
	Color orange;
}//namespace
namespace Level2 {
	void SetupRender()
	{
		// Set the the color to multiply to white, so that the sprite can
		// display the full range of colors (default is black).
		AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

		// Set the color to add to nothing, so that we don't alter the sprite's color
		AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

		// Set blend mode to AE_GFX_BM_BLEND, which will allow transparency.
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		AEGfxSetTransparency(1.0f);
	}
	

	void Init()
	{
		pFont = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);

		windowWidth = static_cast<f32>(AEGfxGetWindowWidth());
		windowHeight = static_cast<f32>(AEGfxGetWindowHeight());

		// Day / Night
		if (DayNightCycle::eDNState == DayNightCycle::STATE_NULL)
		{
			DayNightCycle::Initialize();
			dnState = DayNightCycle::eDNState;
			dayTime = DayNightCycle::MAX_DAY_TIME;
			eveningTime = DayNightCycle::MAX_EVENING_TIME;
			nightTime = DayNightCycle::MAX_NIGHT_TIME;
			skyColor = cyan;
		}
		else
		{
			DayNightCycle::Load(&dnState, &dayTime, &eveningTime, &nightTime);
			switch (dnState)
			{
			case DayNightCycle::STATE_DAY:
				skyColor = cyan;
				break;
			case DayNightCycle::STATE_EVENING:
				skyColor = orange;
				break;
			case DayNightCycle::STATE_NIGHT:
				skyColor = black;
				break;
			case DayNightCycle::STATE_NULL:
				break;
			default:
				break;
			}
		}
		dnPrevState = DayNightCycle::STATE_NULL;
		bSkyColorTransiting = false;
		skyTranisitionTime = DayNightCycle::COLOR_TRANSITION_TIME;
	}

	void Set()
	{
	}

	void Update()
	{
		// TEMP TEST
		if (AEInputCheckTriggered(AEVK_ESCAPE))
		{
			GSM::stateNext = GS_QUIT;
		}
	}

	void Render()
	{
		// Tell the Alpha Engine to set the background to black.
		AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

		// Tell the Alpha Engine to get ready to draw something with color.
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);

		SetupRender();

		const char* pText = "Level 2 Press Esc?";
		f32 width; f32 height;
		AEGfxGetPrintSize(pFont, pText, 1.f, &width, &height);
		AEGfxPrint(pFont, pText, -width * 0.5f, -height * 0.5f, 1, 1, 0, 0, 1);

		// Creates a scaling matrix
		AEMtx33 fontScale = { 0 };
		AEMtx33Scale(&fontScale, windowWidth * 1.12f, windowHeight * 1.12f);

		// Create a translation matrix that translates by
		AEMtx33 fontTranslate = { 0 };
		AEMtx33Trans(&fontTranslate, 0, 0);

		// Concatenate the matrices into the 'transform' variable.
		AEMtx33 fontTransform = { 0 };
		AEMtx33Concat(&fontTransform, &fontTranslate, &fontScale);

		AEGfxSetTransform(fontTransform.m);

		std::string str{ "DNstate: " };
		str += std::to_string(dnState);
		str += " / ";
		str += std::to_string(dnPrevState);
		AEGfxPrint(pFont, str.c_str(), -0.95f, 0.9f, 0.5f, 0, 1, 0, 1);

		str = "Day Time: ";
		str += std::to_string(dayTime);
		AEGfxPrint(pFont, str.c_str(), -0.95f, 0.8f, 0.5f, 0, 1, 0, 1);
		str = "Evening Time: ";
		str += std::to_string(eveningTime);
		AEGfxPrint(pFont, str.c_str(), -0.95f, 0.7f, 0.5f, 0, 1, 0, 1);
		str = "Night Time: ";
		str += std::to_string(nightTime);
		AEGfxPrint(pFont, str.c_str(), -0.95f, 0.6f, 0.5f, 0, 1, 0, 1);
	}

	void Unset()
	{
	}

	void Exit()
	{
		// free font
		AEGfxDestroyFont(pFont);
	}
}

