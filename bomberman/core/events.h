#pragma once


namespace bomberman
{
	enum EventType : int {
		WALK_ON_EXIT  = 0,
		ENEMY_KILLED  = 1,
		TILE_BROKEN   = 2,
		BOMB_CHAINED  = 3,
		EXIT_BOMBED	  = 4,
	};

	typedef void Event;

	struct EventData
	{
		void* data			= nullptr;
		void* eventCaller	= nullptr;
	};

	typedef Event(*EventListener)(const EventData&);

	namespace Events
	{
		void AddListener(const EventType& type, EventListener funcPtr);
		void ClearListeners();

		void CallEvent(const EventType& type, const EventData& data);
	}
}