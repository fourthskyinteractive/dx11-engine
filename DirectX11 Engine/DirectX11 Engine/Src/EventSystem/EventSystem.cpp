/***********************************************
 * Filename:  		EventSystem.cpp
 * Date:      		08/07/2012
 * Mod. Date: 		08/21/2012
 * Mod. Initials:	JF
 * Author:    		Joseph Fink
 * Purpose:   		Receives incoming events and  
 *				sends them to any class that is
 *				registered to receive the events
 *			
 ************************************************/
#include "EventSystem.h"

multimap<EVENTID, EventHandler>		EventSystem::clientDatabase;
queue<Event>						EventSystem::currentEvents;

void EventSystem::DispatchEvent( Event* _event )
{
	if(_event == NULL)
	{
		return;
	}

	pair<multimap<EVENTID, EventHandler>::iterator, multimap<EVENTID, EventHandler>::iterator> range;

	range = clientDatabase.equal_range(_event->GetEventID());

	for(multimap<EVENTID, EventHandler>::iterator mmIter = range.first; mmIter != range.second; mmIter++)
	{
		(*mmIter).second(_event);
	}
}

bool EventSystem::IsAlreadyRegistered( EVENTID _eventID, EventHandler _client )
{
	bool isAlreadyRegistered = false;

	pair<multimap<EVENTID, EventHandler>::iterator, multimap<EVENTID, EventHandler>::iterator> range;

	range = clientDatabase.equal_range(_eventID);

	for(multimap<EVENTID, EventHandler>::iterator mmIter = range.first; mmIter != range.second; mmIter++)
	{
		if((*mmIter).second == _client)
		{
			isAlreadyRegistered = true;
			break;
		}
	}

	return isAlreadyRegistered;
}

void EventSystem::RegisterClient( EVENTID _eventID, EventHandler _client )
{
	if (_client && !IsAlreadyRegistered(_eventID, _client))	
	{
		clientDatabase.insert( make_pair(_eventID, _client) );	
	}	
}

void EventSystem::UnregisterClient( EVENTID _eventID, EventHandler _client )
{
	pair<multimap<EVENTID, EventHandler>::iterator, multimap<EVENTID, EventHandler>::iterator> range;

	range = clientDatabase.equal_range(_eventID);

	for(multimap<EVENTID, EventHandler>::iterator mmIter = range.first; mmIter != range.second; mmIter++)
	{
		if((*mmIter).second == _client)
		{
			clientDatabase.erase(mmIter);
			break;
		}
	}
}

void EventSystem::SendEvent( EVENTID _eventID, void* _data )
{
	Event newEvent(_eventID, _data);
	currentEvents.push(newEvent);
}

void EventSystem::ProcessEvents()
{
	while(currentEvents.size())
	{
		DispatchEvent(&currentEvents.front());
		currentEvents.pop();
	}
}

void EventSystem::ClearEvents()
{
	currentEvents.empty();
}

void EventSystem::ShutdownEventSystem()
{
	clientDatabase.clear();
	currentEvents.empty();
}
