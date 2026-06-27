#pragma once
#include "../rendering/renderer.h"
#include "physics.h"

#ifndef MAP_SIZE
#define MAP_SIZE 1
#define MAP_WIDTH 27
#define MAP_HEIGHT 13
#endif

//#define EMPTY_MAP
//#define DEBUG_MAP
//#define CHASE_MAP
//#define NUKE_ENEMIES

namespace bomberman
{
	class Enemy;
	class Player;
	static const float2 tileInitialOffset{ 20, 40 };
	static const int2 tileSize{32,  32};


	//Converts a normal position to the grid coordinate space
	
	inline int2 NormalSpaceToGridSpace(const float2& position) {
		const int x = static_cast<const int>(floor((position.x) / 32));
		const int y = static_cast<const int>(floor((position.y) / 32));
		return (int2{ x, y }  *int2{ 32, 32 }) + int2{ 4, 25 };
	}

	namespace LevelManager
	{
		// level1 == id = 1
		bool LoadLevel(int levelId, bomberman::World* world);
		void CleanUp();

		void PrepareBatch(Renderer& renderer);
		void RenderBreakableTiles(Renderer& renderer, Camera* camera);

		void CreateNewTile(const int2& tileSprite, TileType type=EMPTY);
		void UpdateMap(float deltaTime);
		void SpawnSpecialTiles(int level);


		//type = new Valcom, etc;
		void SpawnEnemy(const int2& index, Enemy* instance=nullptr);
		bool CanSpawnEnemy();

		//Explodes all tiles  EXPERIMENTAL!
		void Nuke();
		//Swaps out all of the enemies for difficult ones
		void TimeOver();

		bool AnyBreakableTilesPresent();
		bool AnyEnemiesAlive();
		bool IsCollidingWithAnyEnemy(Entity& entity,const PhysicsObject& object, bool autoKill=false, bool pixelPerfect = false, Renderer* renderer=nullptr);

		Tile& GetTile(const int2& index);
		Exit* GetExit();
		


	};


}
