#ifndef EVENTLISTENER_H_
#define EVENTLISTENER_H_

class Event;

class EventListener
{
public:
	EventListener() {}
	virtual ~EventListener() {}
	virtual void HandleEvent(Event* _event) = 0;
};

#endif