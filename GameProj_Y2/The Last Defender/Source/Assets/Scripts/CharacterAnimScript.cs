/******************************************************************************/
/*!t
\file   PlayerAnimScript.cs
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   10/22/2024

\author Matthew Chan Shao jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
	This file contains the class PlayerAnimScript, which is used to handle Player
    animations.

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

public class CharacterAnimScript : EID
{
	CharacterAnimScript()
	{
	}

    // This method is called once when the script is first initialized
    public void OnCreate()
    {
       
    }

    // This method is called once per frame
    void OnUpdate(float dt)
    {
        //Animator animator = gameObject.GetComponent<Animator>();
        //animator.SetLooping(true);
        //animator.SetAnimation("Player_Walk");
        // Update logic here
    }
    public void SetAnimation(string animation, bool looping = true)
    {
        Animator animator = gameObject.GetComponent<Animator>();
        animator.SetLooping(looping);
        animator.SetAnimation(animation);
    }

}
