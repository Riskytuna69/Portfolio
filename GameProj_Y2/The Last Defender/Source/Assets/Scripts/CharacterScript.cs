/******************************************************************************/
/*!
\file   CharacterScript.cs
\par    Project: 7percent
\par    Course: CSD2401
\par    Section B
\par    Software Engineering Project 3
\date   10/22/2024

\author Matthew Chan Shao jie (100%)
\par    email: m.chan\@digipen.edu
\par    DigiPen login: m.chan

\brief
	This file contains the class CharacterScript, which will be used in the future
	to handle Character movement.

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

public class CharacterScript : EID
{
	//Weapons
	private Vector2 aimDirection;
	public bool isFiring;
	public bool releaseWhenCycling;

	//Movement
	private float speed = 500.0f;
	private float accel = 100.0f;
	private float airaccel = 10.0f;
	public float horizontalMovement;
	public float slowdownFactor = 10.0f;

	//Jumping
	private float floorDistance = 1.0f;//! Maximum distance away from the floor that the character can be to still be grounded
	public float groundDistance { get; private set; }//! How far away the floor currently is
	private float jumpPower = 100.0f;
	public bool jumpHeld = false;
	public bool isGrounded { get; private set; }
	private int jumpLeniencyFrames = 0;

	//Dashing
	private float dashPower = 0;
	private bool chargingDash = false;
	private bool canChargeDash = false;
	private int remainingDashes = 0;
	public int totalDashes = 5;

	//Audio control
	public float footstepInterval = 0.1f;
	public float movementVolume = 0.7f;
	private float timeSinceLastFootstep = 0.0f;

	//Animation control
	public string characterAnimationName;//! The name to be used with animations

	//Object references
	Physics body;
	public GameObject raycastDebug;
	CharacterAnimScript animScript;
	Pivot armPivot;

	CharacterScript()
	{
	}

	// This method is called once when the script is first initialized
	void OnStart()
	{
		raycastDebug = GameObject.Find("Raycast Debug");
		animScript = gameObject.GetScriptInChildren<CharacterAnimScript>();
		armPivot = gameObject.GetScriptInChildren<Pivot>();
	}

	// This method is called once per frame
	void OnUpdate(float dt)
	{
		// If the game is paused, return
		if (GameManager.GetPaused())
		{
			return;
		}
		
		--jumpLeniencyFrames;

		body = gameObject.GetComponent<Physics>();

		//Clamp horizontal movement
		horizontalMovement = Mathf.Clamp(horizontalMovement, -1.0f, 1.0f);

		// Grounded check
		isGrounded = false;
		RaycastHit hit;
		Raycasting.TestRaycast(transform.position, new Vector2(0.0f, -1.0f), 0b00001010, out hit);
		float groundDistance = hit.distance;
		if (hit.entity != 0)
		{
			// Check if the ground hit distance is less than the floor distance(plus the height of the player's midpoint)
			if (groundDistance < 15.0f + floorDistance)
			{
				isGrounded = true;
				jumpLeniencyFrames = 8;
			}

			// Debugg Logging
			// Debug.Log("POS AT " + transform.position.x.ToString() + "," + transform.position.y.ToString());
			// Debug.Log("HIT AT " + hit.point.x.ToString() + "," + hit.point.y.ToString() + " - " + hit.distance.ToString()+" and ig = " +isGrounded.ToString());
		}

		// If holding jump
		if (jumpHeld)
		{
			Debug.Log(body.velocity.y.ToString());
			// Jump if player is grounded
			if ((isGrounded || jumpLeniencyFrames >= 0))
			{
				if (body.velocity.y <= 0.5f)
				{
					Jump(); // characterReference.Jump();

					// Play audio here
					AudioManager.StartGroupedSound(movementVolume, "GJ");
				}
			}
			else // Attempt to dash
			{
				if (canChargeDash && remainingDashes > 0)
				{
					chargingDash = true;
				}
			}
		}
		// If space has been released while charging a dash
		if (jumpHeld && chargingDash && remainingDashes > 0)
		{
			--remainingDashes;
			Dash();

			// Play audio here
			AudioManager.StartSingleSound(movementVolume, "Dash");
		}

		// If walking on ground
		if (isGrounded && horizontalMovement != 0.0f)
		{
			timeSinceLastFootstep += dt;
		}
		if (horizontalMovement == 0.0f) // Reset interval if walking stopped
		{
			timeSinceLastFootstep = 0.0f;
		}
		if (timeSinceLastFootstep >= footstepInterval)
		{
			timeSinceLastFootstep = 0.0f;
			AudioManager.StartGroupedSound(movementVolume, "DR");
			Debug.Log("Footstep sound played!");
		}

		// If grounded, set flags
		if (isGrounded)
		{
			canChargeDash = false;
			chargingDash = false;
			remainingDashes = totalDashes;
		}
		else if (!jumpHeld)// Allow dashing
		{
			canChargeDash = true;
		}

		// Clamp horizontal movement
		Vector2 newVelocity = body.velocity;
		if (horizontalMovement == 0)
			newVelocity.x -= dt * body.velocity.x * slowdownFactor * (isGrounded ? 1.0f : airaccel / accel);
		else
		{
			newVelocity.x += dt * 200.0f * (isGrounded ? accel : airaccel) * horizontalMovement;
			newVelocity.x = Mathf.Clamp(newVelocity.x, -speed, speed);
		}
		body.velocity = newVelocity;

		//Anim Component
		if (animScript != null)
		{
			if (isGrounded)// Player is on the ground
			{
				if (horizontalMovement != 0.0f)  // If the player is attempting to move, play walk animation
				{
					animScript.SetAnimation(characterAnimationName + "_Walk");
					return;
				}
				else // Otherwise play the idle animation
				{
					animScript.SetAnimation(characterAnimationName + "_Idle");
					return;
				}
			}
			else// Player is in air/jumping
			{
				if (groundDistance > 100.0f) // High jump animation when the player is far from the grpund
				{
					animScript.SetAnimation(characterAnimationName + "_Jump_High");
				}
				else if (groundDistance > 50.0f) // Low/anticipation animation when the player is near the ground
				{
					animScript.SetAnimation(characterAnimationName + "_Jump_Low");
				}
			}
		}
	}
	public void Dash()
	{
		//Cancel out all movement, I may rewrite this system in the future so you can stack dashes in the same direction for more speed
		// body.velocity = aimDirection * dashPower;
		if(body.velocity.y < jumpPower)
		body.velocity = new Vector2(body.velocity.x, jumpPower);
	}
	public void Jump()
	{
		if (GameManager.GetPlayerJumpEnhanced())
		{
			body.velocity = new Vector2(body.velocity.x, jumpPower * 1.9f);
			AudioManager.StartSingleSound(movementVolume, "JumpEnhanced");
		}
		else
		{
			body.velocity = new Vector2(body.velocity.x, jumpPower);
		}
	}
	public void AimAt(Vector2 target)
	{
		aimDirection = (target - transform.position).normalized;
		armPivot.RotateTowards(target);
	}
}
