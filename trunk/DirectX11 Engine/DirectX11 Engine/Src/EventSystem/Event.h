/***********************************************
 * Filename:  		Event.h
 * Date:      		08/07/2012
 * Mod. Date: 		08/21/2012
 * Mod. Initials:	JF
 * Author:    		Joseph Fink
 * Purpose:   		These are the events that will 
 *				be sent around by the event system.
 *			
 ************************************************/
#ifndef EVENT_H_
#define EVENT_H_

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
using namespace DirectX;
using namespace DirectX::PackedVector;

enum EVENTID { EVENT_SEND_INPUT, EVENT_RECEIVE_INPUT, EVENT_MOUSE_MOVE, EVENT_MOUSE_UPDATE, EVENT_MOUSE_CL};

class Event
{
private:
	EVENTID		eventID;	//	The event's id type.
	void*		param;		//	Store any variable type, mostly a struct related to an event

public:
	Event(EVENTID _eventID, void* _param = 0) : eventID(_eventID), param(_param){}
	~Event(){}

	EVENTID	GetEventID()	{ return eventID; }	// Returns the events ID
	void*	GetParam()		{ return param; }	// Returns the data stored in the event
};


// Ensure that event structures are single byte aligned so they can be easily sent over the network
#pragma pack(push)
#pragma pack(1)

class InputEvent
{
private:
	char playerID;
	short inputFlags;
	
public:
	InputEvent() : inputFlags(0), playerID(-1){}

	void SetPlayerID(int _playerID) {playerID = _playerID;}
	void SetInputFlags(short _inputFlags) {inputFlags = _inputFlags;}

	int GetPlayerID() const {return playerID;}
	short GetInputFlags() const {return inputFlags;}
};

class MouseEvent
{
private:
	char playerID;
	XMHALF2 deltaMouse;

public:
	MouseEvent() : deltaMouse(0.0f, 0.0f), playerID(-1){}

	void SetPlayerID(int _playerID) {playerID = _playerID;}
	void SetMouseDelta(float _deltaX, float _deltaY) {deltaMouse.x = XMConvertFloatToHalf(_deltaX); deltaMouse.y = XMConvertFloatToHalf(_deltaY);}

	int GetPlayerID() const {return playerID;}
	float GetDeltaX() const {return XMConvertHalfToFloat(deltaMouse.x);}
	float GetDeltaY() const {return XMConvertHalfToFloat(deltaMouse.y);}
};

#pragma pack(pop)

#endif