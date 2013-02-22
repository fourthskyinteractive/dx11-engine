#include "DebugOutput.h"
#include <conio.h>

HANDLE DebugOutput::hConsole;
unsigned int DebugOutput::flags;

void DebugOutput::Initialize(unsigned int _flags)
{
	flags = _flags;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}

void DebugOutput::Print(DebuggerFlags _flags,DebugOutputType _type, const char* const _msg)
{
	if(_flags & flags)
	{
		switch(_type)
		{
		case INFO:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			cout << "INFO: " << _msg << "\n";
			break;
		case EVENT:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			cout << "EVENT: " << _msg << "\n";
			break;
		case SUCCESS_CREATE:
			SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			cout << "CREATE: " << _msg << " : SUCCESS\n";
			break;
		case FAILURE_CREATE:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
			cout << "CREATE: " << _msg << " : FAILURE\n";
			break;
		case SUCCESS_DESTROY:
			SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			cout << "DESTROY: " << _msg << " : SUCCESS\n";
			break;
		case FAILURE_DESTROY:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
			cout << "DESTROY: " << _msg << " : FAILURE\n";
			break;
		case FAILURE_SHADER_COMPILE:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
			cout << "SHADER COMPILE: " << _msg << " : FAILURE\n";
			break;
		case NETWORKING:
			SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			cout << "NETWORK: " << _msg << " : SUCCESS\n";
			break;
		case NETWORKING_FAILURE:
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
			cout << "NETWORK: " << _msg << " : FAILURE\n";
			break;

		}
	}
}

void DebugOutput::Clear()
{
	system("cls");
}