/******************************************************************************/
/*!
\file   Reticle.cs
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   27/11/2024

\author Chan Kuan Fu Ryan (100%)
\par    email: c.kuanfuryan\@digipen.edu
\par    DigiPen login: c.kuanfuryan

\brief
  Gets the mouse position and sticks the attached entity's position there.

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

public class Reticle : EID
{
	public static Reticle instance = null;
	public Vector2 position;
	Reticle()
	{
	}

	// This method is called once when the script is first initialized
	public void OnCreate()
	{

	}

	void OnStart()
	{
		if (instance != null)
		{
			Debug.Log("Multiple Reticle instances!");
		}
		instance = this;
	}

	// This method is called once per frame
	void OnUpdate(float dt)
	{
		if (!GameManager.GetPaused())
			MoveTheMouse();
	}

	void LateUpdate(float dt)
	{
		if (GameManager.GetPaused())
			MoveTheMouse();

	}

	void MoveTheMouse()
	{
		Transform t = gameObject.GetComponent<Transform>();
		t.position = Input.GetMousePos();
		position = t.position;
		gameObject.SetTransform(t);
	}
}
