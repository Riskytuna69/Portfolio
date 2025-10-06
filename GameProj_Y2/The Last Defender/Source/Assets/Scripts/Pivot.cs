/******************************************************************************/
/*!
\file   Pivot.cs
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
  Rotates the attached entity depending on relative mouse position.

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

public class Pivot : EID
{
	
	Pivot()
	{
	}

    // This method is called once when the script is first initialized
    public void OnCreate()
    {
       
    }

    // This method is called once per frame
    void OnUpdate(float dt)
    {
	}

	// Rotates the pivot to the direction
	/*****************************************************************//*!
	\brief
		Rotates this gameobject to face a certain direction.
	\param dir
		Direction of Vector2 to face.
	*//******************************************************************/
	public void Rotate(Vector2 dir)
	{
		dir.x *= Mathf.Sign(transform.scale.x);
		double angle = Math.Atan2(dir.y, dir.x) * 180.0 / Math.PI;
		transform.rotation = (float)angle;
	}
	public void RotateTowards(Vector2 target)
	{
		Rotate(target - transform.position);
	}
}
