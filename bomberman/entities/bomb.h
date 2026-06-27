#pragma once
#include "../core/entity.h"
#include "../core/physics.h"
#include "../misc/utils.h"

namespace bomberman
{

	enum ExplosionType: uint8_t
	{
		CENTER = 0,
		MIDDLE = 1,
		END = 2,
	};

	enum ExplosionDirection: uint8_t
	{
		NORTH = 0,
		SOUTH= 1,
		WEST = 3,
		EAST = 2,
	};

	static int2 explosionKeyFrames[3][4] = {
		{{2, 6}, {2, 6}, {2, 6}, {2, 6}}, // center
		{{2, 5}, {2, 7}, {1, 6}, {3, 6}}, // middle
		{{2, 4}, {2, 8}, {4, 6}, {0, 6}} // END
	};

	static int2 explosionOffset[3][4]= {
		{{0, 0}, {0, 0}, {0, 0}}, //center
		{{0, -1}, {0, 1}, {-1, 0}, {1, 0}}, // middle
		{{0, -1}, {0, 1}, {-1, 0}, {1, 0}}, // end
	};


	class Explosion: public AnimatedEntity
	{

	public:
		Explosion() = default;
		~Explosion() override = default;

		void Initialize() override;
		void Tick(float deltaTime) override;

		ExplosionType GetType() const { return type; };
		ExplosionDirection GetDirection() const { return direction; }

		void SetType(ExplosionType nType) { type = nType;  };
		void SetDirection(ExplosionDirection nDir) { direction = nDir;  }

		void SetParent(Entity* entity) { parent = entity; }

	private:
		Entity* parent{nullptr};
		ExplosionType type{END};
		ExplosionDirection direction{ WEST };
		int state = 0;
		int animationState = 0;


		Timer timer{};
	};

	enum BombState: uint8_t
	{
		PASSIVE = 0, // The bomb does not do anything
		ACTIVATED = 1, // the bomb is ticking until it explodes
		EXPLODING = 2, // The explosion is playing
		EXPLODED = 3, // Everything is cleared from the screen.
	};


	class Bomb: public AnimatedEntity
	{
	public:
		static inline int explosionRange{2};

		Bomb() = default;
		~Bomb() override;

		void PrepareRender() override;
		void Initialize() override;
		void Tick(float deltaTime) override;

		void SetParent(Entity* nParent) { parent = nParent;  }

		void Explode(); // immediately explodes the bomb
		void Activate(); // First plays the animation then explodes.
		void Clear(); // Clears both the bomb and explosion.

		BombState GetState() const { return state;  };


	private:
		float explosionColSizes[4] = {2.75f, 2.75f, 2.75f, 2.75f};
		const int collisionSize = 16;
		Timer timer{};
		BombState state{PASSIVE};
		
		int stage = 0;
		utils::vector<Explosion*> explosions{};
		PhysicsObject explosionTriggerNorth{};
		PhysicsObject explosionTriggerSouth{};
		PhysicsObject explosionTriggerWest{};
		PhysicsObject explosionTriggerEast{};

		Entity* parent{ nullptr };
	};

}
