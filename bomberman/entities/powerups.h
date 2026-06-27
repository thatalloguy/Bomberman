#pragma once
#include "../core/entity.h"

namespace bomberman {
	enum class PowerUpType
	{
		SPEED = 0,
		BOMB = 1
	};
	class PowerUp : public BreakableTile
	{
	public:
		PowerUp() = default;
		~PowerUp() override = default;

		void Explode() override;
		void Tick(float deltaTime) override;

		virtual void Activate() = 0;
		virtual int2 GetPowerUpSprite() const { return { 0, 0 }; }


	protected:
		bool exposed = false;
	};

	class SpeedPowerUp : public PowerUp
	{
	public:
		SpeedPowerUp() = default;

		void Activate() override;

		int2 GetPowerUpSprite() const override { return { 2, 14 }; }

	protected:
		bool exposed = false;
	};


	class BombPowerUp : public PowerUp
	{
	public:
		BombPowerUp() = default;

		void Activate() override;

		int2 GetPowerUpSprite() const override { return { 0, 14 }; }

	protected:
		bool exposed = false;
	};

	class BombWalkPowerUp: public PowerUp
	{
	public:
		BombWalkPowerUp() = default;

		void Activate() override;

		int2 GetPowerUpSprite() const override { return {5, 14}; };
	};

	class WallWalkPowerUp : public PowerUp
	{
	public:
		WallWalkPowerUp() = default;

		void Activate() override;

		int2 GetPowerUpSprite() const override { return { 3, 14 }; };
	};

	class MysteryPowerUp : public PowerUp
	{
	public:
		MysteryPowerUp() = default;

		void Activate() override;
		void Tick(float deltaTime) override;

		int2 GetPowerUpSprite() const override { return { 7, 14 }; };

	private:
		Timer timer;
		bool active = false;
	};

	class DetonatorPowerUp: public PowerUp
	{
	public:
		DetonatorPowerUp() = default;

		void Activate() override;
		void Tick(float deltaTime) override;

		int2 GetPowerUpSprite() const override { return { 4, 14 }; };

	private:
		bool isActive = false;
	};


	class FlameProofPowerUp: public PowerUp
	{
	public:
		FlameProofPowerUp() = default;

		void Activate() override;
		int2 GetPowerUpSprite() const override { return { 6, 14 }; };

	};

	class FlamePowerUp: public PowerUp
	{
	public:
		FlamePowerUp() = default;

		void Activate() override;
		int2 GetPowerUpSprite() const override { return { 1, 14 }; };
	};

}