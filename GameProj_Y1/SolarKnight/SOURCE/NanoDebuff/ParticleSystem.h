/*!************************************************************************
\file       ParticleSystem.h
\project    Solar Knight
\author(s)  Gavin Ang Jun Liang (ang.g) (100%)

All content © 2024 DigiPen Institute of Technology Singapore.
All rights reserved.
**************************************************************************/

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "pch.h"
#include "CreateMesh.h"
#include "GameObject.h"
#include "Color.h"
#include <array>

struct ParticleProperties {
	AEVec2 position;
	AEVec2 velocity;
	AEVec2 velocityVariation;
	f32 sizeBegin;
	f32 sizeEnd; 
	f32 sizeVariation;
	f32 lifeTime;
	Colors::Color color;
};

struct BuffParticleProperties {
	AEVec2 position;
	AEVec2 velocity;
	AEVec2 velocityVariation;
	f32 sizeBegin;
	f32 sizeEnd;
	f32 sizeVariation;
	f32 lifeTime;
	Colors::Color color;
};

class ParticleSystem
{
public:
	ParticleSystem();
	ParticleSystem(ParticleSystem const&) = delete;
	ParticleSystem& operator=(ParticleSystem const&) = delete;
	~ParticleSystem();


	void OnParticleUpdate(f32 time);
	void OnParticleRender(AEGfxVertexList* pMeshParticle);
	void Emit(const ParticleProperties& particleProperties);

	void OnBuffParticleUpdate(f32 time, GameObject::PlayerObject* player);
	void OnBuffParticleRender(AEGfxVertexList* pMeshParticle);
	void EmitBuffParticle(const BuffParticleProperties& buffParticleProperties);

	void OnParticleSetFalse();

	void SpawnBulletParticle(GameObject::GameObject* bulletObj);
	void SpawnEnemyParticle(f32 enemyX, f32 enemyY);
	void SpawnBuffParticle(GameObject::GameObject* drops, GameObject::PlayerObject* player);

	void CleanParticle();


	
private:
	struct Particle{
		AEVec2 position;
		AEVec2 velocity;
		f32 sizeBegin;
		f32 sizeEnd;
		f32 lifeTime;
		f32 lifeRemaining;
		bool active;
		Colors::Color color;
	};

	std::array<Particle, 100> particlePool{};
	std::array<Particle, 100> buffParticlePool{};

	int poolIndex = 99;
	int buffPoolIndex = 99;


	ParticleProperties particle;
	BuffParticleProperties buffParticle;
};


#endif // PARTICLESYSTEM_H