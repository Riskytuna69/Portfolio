/*!************************************************************************
\file       ParticleSystem.cpp
\project    Solar Knight
\author(s)  Gavin Ang Jun Liang (ang.g) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	//initialize particle
	AEVec2 tempPos{ 0.0f,0.0f };
	AEVec2 tempVelocity{ 50.0f,50.0f };
	AEVec2 tempVelocityVariation{ 30.0f,30.0f };
	particle.position = tempPos;
	particle.velocity = tempVelocity;
	particle.velocityVariation = tempVelocityVariation;
	particle.sizeBegin = 50.0f;
	particle.sizeEnd = 0.0f;
	particle.sizeVariation = 3.0f;
	particle.lifeTime = 1.0f;

	
	//initialize buff particle
	AEVec2 tempPosBuff{ 0.0f,0.0f };
	AEVec2 tempVelocityBuff{ 50.0f,100.0f };
	AEVec2 tempVelocityVariationBuff{ 30.0f,30.0f };
	buffParticle.position = tempPosBuff;
	buffParticle.velocity = tempVelocityBuff;
	buffParticle.velocityVariation = tempVelocityVariationBuff;
	buffParticle.sizeBegin = 50.0f;
	buffParticle.sizeEnd = 0.0f;
	buffParticle.sizeVariation = 3.0f;
	buffParticle.lifeTime = 1.0f;
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::OnParticleUpdate(f32 time)
{
	for (ParticleSystem::Particle& particles : particlePool)
	{
		//if particle is not active continue
		if (!particles.active)
			continue;

		//if particle life remaining is less than or equal to zero
		//set active to false and continue
		if (particles.lifeRemaining <= 0.0f)
		{
			particles.active = false;
			continue;
		}

		particles.lifeRemaining -= time;
		particles.position.x +=  particles.velocity.x * (time);
		particles.position.y +=  particles.velocity.y * (time);
	}
}

void ParticleSystem::OnParticleRender(AEGfxVertexList* pMeshParticle)
{

	for (ParticleSystem::Particle& particles : particlePool)
	{

		if (!particles.active)
			continue;

		//fade away particle
		float life = particles.lifeRemaining / particles.lifeTime;

		float size = particles.sizeBegin * life;

		AEMtx33 scale{ 0 };
		AEMtx33 translate{ 0 };
		AEMtx33 transform{ 0 };

		// Scale the tile
		AEMtx33Scale(&scale, size, size);

		// Translate to center tiles
		AEMtx33Trans(&translate, particles.position.x, particles.position.y);

		// Get matrix
		AEMtx33Concat(&transform, &translate, &scale);

		// Set the texture to enemyPng, detected!
		AEGfxSetColorToMultiply(particles.color.r, particles.color.g, particles.color.b, 1);

		// Tell Alpha Engine to use the matrix in 'transform' to apply onto all
		// the vertices of the mesh that we are about to choose to draw in the next line.
		AEGfxSetTransform(transform.m);

		AEGfxMeshDraw(pMeshParticle, AE_GFX_MDM_TRIANGLES);
	}
}

void ParticleSystem::Emit(const ParticleProperties& particleProperties)
{
	Particle& particles = particlePool[poolIndex];


	particles.active = true;
	particles.position.x = particleProperties.position.x;
	particles.position.y = particleProperties.position.y;

	//velocity
	particles.velocity = particleProperties.velocity;
	particles.velocity.x = particleProperties.velocityVariation.x * (AERandFloat() * 2 - 1);
	particles.velocity.y = particleProperties.velocityVariation.y * (AERandFloat() * 2 - 1);

	//size
	particles.lifeTime = particleProperties.lifeTime;
	particles.lifeRemaining = particleProperties.lifeTime;
	particles.sizeBegin = particleProperties.sizeBegin;
	particles.sizeEnd = particleProperties.sizeEnd;

	particles.color = particleProperties.color;

	poolIndex = --poolIndex % particlePool.size();
}

void ParticleSystem::OnBuffParticleUpdate(f32 time, GameObject::PlayerObject* player)
{
	for (ParticleSystem::Particle& particles : buffParticlePool)
	{
		//if particle is not active continue
		if (!particles.active)
			continue;

		//if particle life remaining is less than or equal to zero
		//set active to false and continue
		if (particles.lifeRemaining <= 0.0f)
		{
			particles.active = false;
			continue;
		}
		f32 offsetPlayer = player->go->position.y - 25.0f;
		f32 offsetParticle = particles.position.y - offsetPlayer;
		particles.lifeRemaining -= time;
		particles.position.x = player->go->position.x + (particles.velocity.x * (time) * 50.0f);
		particles.position.y = (player->go->position.y - 25.0f) + (particles.velocity.y * (time)) + abs(offsetParticle);
	}
}

void ParticleSystem::OnBuffParticleRender(AEGfxVertexList* pMeshParticle)
{

	for (ParticleSystem::Particle& particles : buffParticlePool)
	{

		if (!particles.active)
			continue;

		//fade away particle
		float life = particles.lifeRemaining / particles.lifeTime;

		float size = particles.sizeBegin * life;

		AEMtx33 scale{ 0 };
		AEMtx33 translate{ 0 };
		AEMtx33 transform{ 0 };

		// Scale the tile
		AEMtx33Scale(&scale, size, size);

		// Translate to center tiles
		AEMtx33Trans(&translate, particles.position.x, particles.position.y);

		// Get matrix
		AEMtx33Concat(&transform, &translate, &scale);

		// Set the texture to enemyPng, detected!
		AEGfxSetColorToMultiply(particles.color.r, particles.color.g, particles.color.b, 1);

		// Tell Alpha Engine to use the matrix in 'transform' to apply onto all
		// the vertices of the mesh that we are about to choose to draw in the next line.
		AEGfxSetTransform(transform.m);

		AEGfxMeshDraw(pMeshParticle, AE_GFX_MDM_TRIANGLES);
	}
}

void ParticleSystem::EmitBuffParticle(const BuffParticleProperties& buffParticleProperties)
{
	Particle& particles = buffParticlePool[buffPoolIndex];
	particles.active = true;
	particles.position.x = buffParticleProperties.position.x;
	particles.position.y = buffParticleProperties.position.y;

	//velocity
	particles.velocity = buffParticleProperties.velocity;
	particles.velocity.x = buffParticleProperties.velocityVariation.x * (AERandFloat() * 2 - 1);
	particles.velocity.y = buffParticleProperties.velocityVariation.y * (AERandFloat() * 2 + 1);

	//size
	particles.lifeTime = buffParticleProperties.lifeTime;
	particles.lifeRemaining = buffParticleProperties.lifeTime;
	particles.sizeBegin = buffParticleProperties.sizeBegin /*+ particleProperties.sizeVariation * (AERandFloat() * 2 - 1)*/;
	particles.sizeEnd = buffParticleProperties.sizeEnd;

	particles.color = buffParticleProperties.color;

	buffPoolIndex = --buffPoolIndex % buffParticlePool.size();
}

void ParticleSystem::OnParticleSetFalse()
{
	for (ParticleSystem::Particle& particles : particlePool)
	{
		if (particles.active)
			particles.active = false;
	}
	//CleanParticle();
}

void ParticleSystem::SpawnBulletParticle(GameObject::GameObject* bulletObj)
{
	particle.position.x = static_cast<float>(bulletObj->position.x);
	particle.position.y = static_cast<float>(bulletObj->position.y);
	particle.sizeBegin = 10.0f;
	particle.color = Colors::Color{ 0.5f,0.5f,0.5f };

	for (int i = 0; i < 5; i++)
	{
		ParticleSystem::Emit(particle);
	}
}

void ParticleSystem::SpawnEnemyParticle(f32 enemyX, f32 enemyY)
{
	particle.position.x = enemyX;
	particle.position.y = enemyY;
	particle.sizeBegin = 50.0f;
	particle.color = Colors::Color{ 0.5f,0.5f,0.5f };

	for (int i = 0; i < 10; i++)
	{
		ParticleSystem::Emit(particle);
	}
}

void ParticleSystem::SpawnBuffParticle(GameObject::GameObject* drops, GameObject::PlayerObject* player)
{
	buffParticle.position.y = (player->go->position.y);
	buffParticle.sizeBegin = 25.f;

	switch (drops->type)
	{
	case GameObject::OBJ_ATK_UP:
		// Set to red
		buffParticle.color = Colors::Color{ 1.0f, 0.0f, 0.0f };
		break;
	case GameObject::OBJ_PWR_UP:
		// Set to purple
		buffParticle.color = Colors::Color{ 1.0f, 0.0f, 1.0f };
		break;
	case GameObject::OBJ_SOLAR_UP:
		// Set to orange
		buffParticle.color = Colors::Color{ 1.0f, 0.784313725f, 0.596078431372f };
		break;
	case GameObject::OBJ_HP_REGEN:
		// Set to green
		buffParticle.color = Colors::Color{ 0.0f, 1.0f, 0.0f };
		break;
	case GameObject::OBJ_SOLAR_REGEN:
		// Set to orange
		buffParticle.color = Colors::Color{ 1.0f, 0.6470588235f, 0.0f };
		break;
	default:
		break;
	}

	for (int i = 0; i < 10; i++)
	{
		ParticleSystem::EmitBuffParticle(buffParticle);
	}
}

void ParticleSystem::CleanParticle()
{
	//particlePool.clear();
	//buffParticlePool.clear();
}