#pragma once

#include "../core/entity.h"

//NOTE: UNUSED
namespace bomberman
{

	class Dummy: public AnimatedEntity
	{
	public:
		~Dummy() override = default;

		void Initialize() override;
		void PrepareRender() override;
		void Destroy() override;

	private:
		Timer animationTimer;
	};
}
