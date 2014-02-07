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

	static enum DebuggerFlags {RENDER_MSG = 1, FBXLOADER_MSG = 2};
	static enum DebugOutputType { INFO, SUCCESS_CREATE, FAILURE_CREATE, SUCCESS_GENERIC, FAILURE_GENERIC};

	static void Initialize(unsigned int _flags);
	static void Print(DebuggerFlags _flags, DebugOutputType _type, const char* const _msg);
	static void Clear();

private:
	static unsigned int flags;
	static HANDLE hConsole;
};

#endif