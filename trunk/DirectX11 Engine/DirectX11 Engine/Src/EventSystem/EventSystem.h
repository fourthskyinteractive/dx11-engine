/***********************************************
 * Filename:  		EventSystem.h
 * Date:      		08/07/2012
 * Mod. Date: 		08/21/2012
 * Mod. Initials:	JF
 * Author:    		Joseph Fink
 * Purpose:   		Receives incoming events and  
 *				sends them to any class that is
 *				registered to receive the events
 *			
 ************************************************/
#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include "Event.h"

#include <Windows.h>

#include <queue>
#include <map>
using std::multimap;
using std::pair;
using std::queue;
using std::make_pair;

// Any class receiving an event must have a function that matches this declaration
typedef void (*EventHandler)(Event* _event);

class EventSystem
{
private:
	static multimap<EVENTID, EventHandler>	clientDatabase;
	static queue<Event>						currentEvents;

	static void DispatchEvent(Event* _event);
	static bool IsAlreadyRegistered(EVENTID _eventID, EventHandler _client);

public:
	static void RegisterClient(EVENTID _eventID, EventHandler _client);		// Register client to receive and event
	static void UnregisterClient(EVENTID _eventID, EventHandler _client);	// Unregister client from a specific event	
	static void SendEvent(EVENTID _eventID, void* _data = NULL);			// Receives an event that will be sent to clients
	static void ProcessEvents();											// Dispatches all currently stored events
	static void ClearEvents();												// Clears all the events
	static void ShutdownEventSystem();										// Clears all clients and events 

};

#endif