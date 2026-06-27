#pragma once
#include "../core/entity.h"

namespace bomberman
{
	enum BonusType
	{
		BPANEL		= 0,
		GODDESS		= 1,
		COLA		= 2,
		FAMICOM		= 3,
		NAKAMOTO	= 4,
		DEZENIMAN	= 5,
		ALLO		= 6
	};

	enum BonusState
	{
		INACTIVE	= 0,
		ACTIVE		= 1,
		COMPLETED	= 2,
		FAILED		= 3,
	};

	class Bonus: public BreakableTile
	{
	public:
		Bonus() = default;

		void Initialize() override;
		void Tick(float deltaTime) override;

		bool HasBomb() const override { return type == BONUS ? true : bomb != nullptr;  };

		
	protected:
		BonusState state{ INACTIVE };

		virtual void InactiveTick(float deltaTime)	= 0;
		virtual void ActiveTick(float deltaTime)	= 0;
		virtual void CompletedTick(float deltaTime) = 0;
	};


	class BpanelBonus: public Bonus
	{
	public:
		~BpanelBonus() override = default;
		void Initialize() override;

		void Tick(float deltaTime) override;

	protected:

		void InactiveTick(float deltaTime) override;
		void ActiveTick(float deltaTime) override;
		void CompletedTick(float deltaTime) override;

	};

	//Note: Does NOT support more than 2 players.
	// Any players with a higher ID than the second player are ignored.
	class GoddessBonus: public Bonus
	{
	public:
		~GoddessBonus() override = default;

		void Initialize() override;

		void Tick(float deltaTime) override;

	protected:

		void InactiveTick(float deltaTime) override;
		void ActiveTick(float deltaTime) override;
		void CompletedTick(float deltaTime) override;

	private:
		// 2 arrays because we have 2 players
		bool rowsCompleted[2][4] = {
			{
				false, // north
				false, // east
				false, // south
				false, // west
			},
			{
				false, // north
				false, // east
				false, // south
				false, // west
			},
		};
	};

	class ColaBonus : public Bonus
	{
	public:
		~ColaBonus() override = default;
		void Initialize() override;

		void Tick(float deltaTime) override;

	protected:

		void InactiveTick(float deltaTime) override;
		void ActiveTick(float deltaTime) override;
		void CompletedTick(float deltaTime) override;


	private:
		Timer timer{};
	};

	class FamicomBonus: public Bonus
	{
	public:
		~FamicomBonus() override = default;
		void Initialize() override;

		void Tick(float deltaTime) override;

	protected:

		void InactiveTick(float deltaTime) override;
		void ActiveTick(float deltaTime) override;
		void CompletedTick(float deltaTime) override;


	};

	class NakamotoBonus: public Bonus
	{
	public:
		~NakamotoBonus() override = default;


		void Initialize() override;

	protected:

		void InactiveTick(float deltaTime) override;
		void ActiveTick(float deltaTime) override;
		void CompletedTick(float deltaTime) override;

	};

	class DezenimanBonus: public Bonus
	{
	public:
		~DezenimanBonus() override = default;


		void Initialize() override;

	protected:

		void InactiveTick(float deltaTime) override;
		void ActiveTick(float deltaTime) override;
		void CompletedTick(float deltaTime) override;


	private:
		Timer timeCheck{};
		const float tileCheckDelay = 0.4f;
	};

	
}
