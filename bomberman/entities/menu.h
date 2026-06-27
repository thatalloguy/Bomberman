#pragma once
#include "../core/entity.h"

namespace bomberman
{
	class Arrow: public Entity
	{
	public:
		Arrow() = default;
		~Arrow() override = default;

		void Initialize() override;
		void PrepareRender() override;
		void Tick(float deltaTime) override;
		void Destroy() override;

	};
	class Text: public AnimatedEntity
	{
	public:
		static constexpr float loadTime = 3.0f;
		Text() = default;
		~Text() override = default;

		void Initialize() override;
		void PrepareRender() override;
		void Tick(float deltaTime) override;
		void Destroy() override;

	};
	class Menu: public Entity
	{
	public:
		Menu() = default;
		~Menu() override = default;

		void Initialize() override;
		void PrepareRender() override;
		void Tick(float deltaTime) override;
		void Destroy() override;

	};

}

