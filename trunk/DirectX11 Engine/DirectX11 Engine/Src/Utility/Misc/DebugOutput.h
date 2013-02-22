#ifndef DEBUGOUTPUT_H
#define DEBUGOUTPUT_H

#include<windows.h>
#include<iostream>
#include<sstream>
using std::cout;
using std::stringstream;

class DebugOutput
{
public:

	static enum DebuggerFlags {NETWORK_MSG = 1, RENDER_MSG = 2, PHYSICS_MSG = 4, INPUT_MSG = 8, SOUND_MSG = 16 , ANIMATION_MSG = 32, OTHER_MSG = 256};
	static enum DebugOutputType { INFO, EVENT, SUCCESS_CREATE, FAILURE_CREATE, SUCCESS_DESTROY, FAILURE_DESTROY, FAILURE_SHADER_COMPILE, NETWORKING, NETWORKING_FAILURE };

	static void Initialize(unsigned int _flags);
	static void Print(DebuggerFlags _flags, DebugOutputType _type, const char* const _msg);
	static void Clear();

private:
	static unsigned int flags;
	static HANDLE hConsole;
};

#endif