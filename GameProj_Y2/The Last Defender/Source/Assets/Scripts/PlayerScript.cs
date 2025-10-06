/******************************************************************************/
/*!
\file   PlayerScript.cs
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
  Controller for player movement, shooting and animation.

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

public class PlayerScript : EID
{
	PlayerScript()
	{
	}

	public CharacterScript characterReference;

	Reticle reticleReference = null;

	// Semi Auto Fix will automatically release the trigger when weapon is cycling.
	// Good for allowing enemies to "hold to fire" semi-auto weapon

	// This method is called once when the script is first initialized
	public void OnCreate()
	{
	}

	//public void OnCollision(Collision collision)
	//{
    //    //Debug.Log("BONK "+collision.point.x.ToString()+","+collision.point.y.ToString());
    //    //Debug.Log("BONK FROM " + collision.eid.ToString() + " TO " + collision.other.ToString());
	//	//GameObject.InstantiatePrefab("Bullet");
    //}

    void OnStart()
	{
		reticleReference = GameObject.Find("Reticle")?.GetScript<Reticle>();
		characterReference = gameObject.GetScript<CharacterScript>();
		characterReference.characterAnimationName = "Player";
	}

	// This method is called once per frame
	void OnUpdate(float dt)
	{
		// If the game is paused, return
		if (GameManager.GetPaused())
		{
			return;
		}

		if (Input.GetCurrKey(KeyCode.A))
			characterReference.horizontalMovement = -1.0f;
		else if (Input.GetCurrKey(KeyCode.D))
			characterReference.horizontalMovement = 1.0f;
		else
			characterReference.horizontalMovement = 0.0f;



		// NOTES
		// Eventually we'll be moving a lot of this code back to Character.cs if possible, but for now it's all contained within Player.cs
		// Jump logic, needs isGrounded check
		characterReference.jumpHeld = Input.GetKeyPressed(KeyCode.SPACE);

		// Aim at the reticle
		if (reticleReference != null)
		{
			characterReference.AimAt(reticleReference.position);
		}


	}
}
