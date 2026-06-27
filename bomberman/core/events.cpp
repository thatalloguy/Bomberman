#include "precomp.h"
#include "events.h"


namespace 
{
	bomberman::EventListener eventListeners[4] = {nullptr, nullptr, nullptr, nullptr};
}

void bomberman::Events::AddListener(const EventType& type, EventListener funcPtr)
{
	eventListeners[type] = funcPtr;
}

void bomberman::Events::ClearListeners()
{
	for (int i = 0; i < 4; i++) eventListeners[i] = nullptr;
}

void bomberman::Events::CallEvent(const EventType& type, const EventData& data)
{
	if (eventListeners[type])
		eventListeners[type](data);
}
