#pragma once
#include "assetManager.h"
#include "template/tmpl8math.h"

namespace bomberman
{
	class Bomb;


	// An entity is ANYTHING that can get drawn.
	// So tiles, players, enemies all come down to entities.

	class Entity
	{
	public:
		//Note We the entity do NOT manage the memory of the texture.
		Entity() = default;
		virtual ~Entity() = default;

		virtual void Initialize() = 0;
		virtual void PrepareRender() = 0;
		virtual void Tick(float deltaTime) = 0;
		virtual void Destroy() = 0;

		float2 GetPosition() const { return position;  }
		float2 GetScale() const { return scale; }

		void SetPosition(const float2& newPosition) { position = newPosition;  }
		void SetScale(const float2& newScale) { scale = newScale;  }

		GLTexture* GetTexture() const { return texture; }
		Shader* GetShader() const { return shader;  }

	protected:
		float2 position{ 0, 0 };
		float2 scale{ 0, 0 };

		uint id{ 0 };
		// When the entity gets initialized it itself is responsible for retrieving its desired shader and texture from the Renderer.
		GLTexture* texture{nullptr};
		Shader* shader{nullptr};
		
	};

	//bomberman frame is 25x17
	//An animated entity is a entity that has multiple keyframes stored in their texture.
	// It also uses a different shader.
	class AnimatedEntity : public Entity {
	public:


		~AnimatedEntity() override = default;

		void Initialize() override
		{
			texture = Assets::GetTexture(TextureType::TILES);
			shader = Assets::GetShader(ShaderType::ANIMATED);
		}

		void PrepareRender() override
		{
			shader->SetFloat4("filter", filterColor);
			shader->SetFloat2("grid", frameSize);
			shader->SetFloat2("frame", currentFrame);
			shader->SetFloat4("secondary", secondaryColor);
			shader->SetFloat4("replace", replaceColor);
		}

		void Destroy() override
		{
			
		}

		void Tick(float) override {}

		void SetFrame(const int2& frame)
		{
			currentFrame = frame;
		}


		float2 GetFrameSize() const { return frameSize; }
		float2 GetCurrentFrame() const { return currentFrame; }
		float4 GetFilterColor() const { return filterColor;  }
	protected:
		float4 filterColor{ 0, 1, 0, 0 };
		float4 secondaryColor{ 0, 0, 0, 1 };
		float4 replaceColor{ 0,0, 1, 1};
		float2 frameSize{ 16, 384 };
		float2 currentFrame{ 1, 1 };
	};


	enum TileType: uint
	{
		UNKNOWN = 0, // the missing texture
		EMPTY = 1, // the dark green background tex
		SOLID = 2,
		BREAKABLE = 3,
		EXIT = 4,
		POWERUP = 5,
		BOMB = 6,
		BONUS = 7,
	};

	class Tile : public Entity {
	public:


		~Tile() override;

		void Initialize() override
		{

			texture = Assets::GetTexture(TextureType::TILES);
			shader = Assets::GetShader(ShaderType::TILE);

		}

		void Tick(float deltaTime) override;

		void PrepareRender() override {}
		void Destroy() override {}

		bool IsSolid() const { return type != EMPTY && type != EXIT && type != POWERUP; }



		virtual void SpawnBomb(const float2& spawnPos, bool explode=true);
		virtual Bomb* GetBomb() { return bomb; }
		virtual void RemoveBomb();
		virtual bool HasBomb() const { return bomb != nullptr; };

		virtual void Explode();

		void SetIndex(const float2& newIndex) { index = newIndex; }
		void SetMapIndex(const int2& newIndex) { mapIndex = newIndex; }
		void SetType(TileType newType) { type = newType; }

		float2 GetFrameSize() const { return {16, 384}; }
		float2 GetIndex() const { return index; }
		int2 GetMapIndex() const { return mapIndex; }
		TileType GetType() const { if (HasBomb()) return BOMB; else return type; }

	protected:
		float2 index{ 1, 1 };
		int2 mapIndex{ 1, 1 };
		Bomb* bomb{nullptr};
		TileType type{ UNKNOWN };
	};


	//BreakableTiles are a seperate class because they require custom logic.
	class BreakableTile: public Tile
	{
	public:
		~BreakableTile() override = default;

		void Initialize() override
		{
			type = BREAKABLE;
			index = {4, 3};
			texture = Assets::GetTexture(TextureType::TILES);
			shader = Assets::GetShader(ShaderType::ANIMATED);
		}

		void PrepareRender() override
		{

			shader->SetFloat4("filter", { 0, 1, 0, 1 });
			shader->SetFloat2("grid", { 16, 384 });
			shader->SetFloat2("frame", index);
			shader->SetFloat4("secondary", { 0, 0, 0, 1 });
			shader->SetFloat4("replace", { 0, 0, 0, 1 });
		}

		void Destroy() override
		{

		}

		void Explode() override
		{
			Tile::Explode();

			if (state == 0 && type == BREAKABLE)
			{
				state++;
			}

		};


		void Tick(float deltaTime) override;

	protected:
		// goes from 0 to 6
		int state = 0;
		Timer timer{};

	};

	class Exit: public BreakableTile
	{
	public:
		Exit() = default;
		~Exit() override = default;

		void Explode() override;
		void Tick(float deltaTime) override;
	};


}
