#include "Event.h"

Event::Event(EVENTID _eventID, void* _param) : eventID(_eventID), param(_param)
{
}

Event::~Event()
{
}