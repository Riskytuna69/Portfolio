/******************************************************************************/
/*!
\file   Flipper.cs
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   11/27/2024

\author Chan Kuan Fu Ryan (50%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\author Matthew Chan Shao Jie (50%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
  Flips the attached entity's scale.x depending on relative mouse position.

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

public class Flipper : EID
{
	
	Flipper()
	{
	}

    // This method is called once when the script is first initialized
    public void OnCreate()
    {
       
    }

    // This method is called once per frame
    void OnUpdate(float dt)
    {
		// If the game is paused, return
		if (GameManager.GetPaused())
		{
			return;
		}

		// Update logic here
		Vector2 aimDirection = (Input.GetMousePos() - transform.position);
        transform.localScale = new Vector2(Mathf.Sign(aimDirection.x),1.0f);
        //GetScript<Pivot>();
    }
}
