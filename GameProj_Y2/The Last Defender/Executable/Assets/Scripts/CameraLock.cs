/******************************************************************************/
/*!
\file   CameraLock.cs
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   27/11/2024

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  Sets the camera's zoom at the start of the game.

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
*/
/******************************************************************************/
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using EngineScripting;

public class CameraLock : EID
{
    public float zoom = 2.0f;

	CameraLock()
	{
	}

    // This method is called once when the script is first initialized
    public void OnCreate()
    {
       
    }

    void OnStart()
    {
		Camera.SetZoom(zoom);
	}

    // This method is called once per frame
    void OnUpdate(float dt)
    {
        // Update logic here
    }
}
