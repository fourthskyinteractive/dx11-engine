#include "Input.h"
#include <string>
#include <fstream>
#include <sstream>
#include "../Utility/Misc/DebugOutput.h"
#include "../Game/Definitions.h"
#include<sstream>
using std::stringstream;

using std::ifstream;
using std::ios_base;
using std::istringstream;
using std::string;

#define MOUSE_SHOOT 0

POINT				Input::cursorPos;
POINT				Input::prevCursorPos;
POINT				Input::cursorRelPos;
Key					Input::keys[256];
Key					Input::prevKeys[256];

Key					Input::mouseButtons[8];
Key					Input::prevMouseButtons[8];

CRITICAL_SECTION	Input::criticalSection;

vector<char>		Input::keyBindings;
vector<char>		Input::mouseBindings;

MouseEvent			Input::mouseEvent;
InputEvent			Input::inputEvent;

float				Input::sensitivity;
bool				Input::inverted;

Timer				Input::time;
float				Input::step;
float				Input::limit;

short				Input::inputFlags;
bool				Input::mouseUp;
bool				Input::mouseDown;
bool				Input::prevMouseDown;

bool Input::Init()
{
	LoadKeyBindings("Res/Data/Controls/DefaultBindings.txt");
	//LoadSaveControls("Res/Data/Controls/SavedControls.txt");

	ZeroMemory((void*)&keys,sizeof(Key)*256);
	ZeroMemory((void*)&prevKeys,sizeof(Key)*256);

	InitializeCriticalSection(&criticalSection);

	cursorRelPos.x = 0;
	cursorRelPos.y = 0;

	cursorPos.x = 0;
	cursorPos.y = 0;

	SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);
	
	time.Init();
	limit = 0.25f;
	
	return true;
}

void Input::ClearKeys()
{
	ZeroMemory((void*)&keys,sizeof(Key)*256);

	cursorRelPos.x = 0;
	cursorRelPos.y = 0;
}

void Input::RawKeyInput(RAWKEYBOARD& kb)
{
	EnterCriticalSection(&criticalSection);

	if(kb.Message == WM_KEYDOWN || kb.Message == WM_SYSKEYDOWN)
	{
		keys[kb.VKey].down = true;
	}
	if(kb.Message == WM_KEYUP || kb.Message == WM_SYSKEYUP)
	{
		keys[kb.VKey].down = false;
	}

	CheckSpecialCases();

	LeaveCriticalSection(&criticalSection);
}

void Input::CheckSpecialCases()
{
	if(keys[VK_MENU].down && keys[VK_F4].down)
		PostQuitMessage(0);
}

void Input::RawMouseInput(RAWMOUSE& mouse)
{
	EnterCriticalSection(&criticalSection);
	cursorRelPos.x = mouse.lLastX;
	cursorRelPos.y = mouse.lLastY;

	cursorPos.x+=mouse.lLastX;
	cursorPos.y+=mouse.lLastY;


	switch(mouse.usButtonFlags)
	{
	case RI_MOUSE_BUTTON_1_DOWN:
		{
			mouseButtons[0].down = true;
		}
		break;

	case RI_MOUSE_BUTTON_1_UP:
		{
			mouseButtons[0].down = false;
		}
		break;
	
	case RI_MOUSE_BUTTON_2_DOWN:
		{
			mouseButtons[1].down = true;
		}
		break;

	case RI_MOUSE_BUTTON_2_UP:
		{
			mouseButtons[1].down = false;
		}
		break;

	case RI_MOUSE_BUTTON_3_DOWN:
		{
			mouseButtons[2].down = true;
		}
		break;

	case RI_MOUSE_BUTTON_3_UP:
		{
			mouseButtons[2].down = false;
		}
		break;

	case RI_MOUSE_BUTTON_4_DOWN:
		{
			mouseButtons[3].down = true;
		}
		break;

	case RI_MOUSE_BUTTON_4_UP:
		{
			mouseButtons[3].down = false;
		}
		break;

	case RI_MOUSE_BUTTON_5_DOWN:
		{
			mouseButtons[4].down = true;
		}
		break;

	case RI_MOUSE_BUTTON_5_UP:
		{
			mouseButtons[4].down = false;
		}
		break;
	}

	LeaveCriticalSection(&criticalSection);
}

bool Input::KeyDown(char _keyCode)
{
	EnterCriticalSection(&criticalSection);
	bool result = keys[_keyCode].down;
	LeaveCriticalSection(&criticalSection);

	return result;
}

bool Input::KeyPressed(char _keyCode)
{
	EnterCriticalSection(&criticalSection);
	bool result = keys[_keyCode].down && !prevKeys[_keyCode].down;
	LeaveCriticalSection(&criticalSection);

	return result;
}

char Input::GetKeyPressed()
{
	for(int i = 0; i < 256; ++i)
	{
		if(KeyPressed(i))
		{
			return i;
		}
	}

	return 0;
}

bool Input::KeyReleased(char _keyCode)
{
	EnterCriticalSection(&criticalSection);
	bool result = !keys[_keyCode].down && prevKeys[_keyCode].down;
	LeaveCriticalSection(&criticalSection);
	
	return result;
}

bool Input::MouseButtonPressed(char _button)
{
	EnterCriticalSection(&criticalSection);
	bool result = mouseButtons[_button].down && !prevMouseButtons[_button].down;
	LeaveCriticalSection(&criticalSection);
	return result;
}

bool Input::MouseButtonReleased(char _button)
{
	EnterCriticalSection(&criticalSection);
	bool result = !mouseButtons[_button].down && prevMouseButtons[_button].down;
	LeaveCriticalSection(&criticalSection);
	return result;
}


static short prevInputFlags = 0;
void Input::Update()
{
	
	mouseUp = false;
	prevMouseDown = mouseDown;
	POINT deltaPos;
	deltaPos.x = cursorPos.x - prevCursorPos.x;
	deltaPos.y = cursorPos.y - prevCursorPos.y;

	prevCursorPos = cursorPos;

	float modifiedSensitivity = sensitivity;

	if(inverted)
		mouseEvent.SetMouseDelta(deltaPos.x / modifiedSensitivity, deltaPos.y / -modifiedSensitivity);
	else
		mouseEvent.SetMouseDelta(deltaPos.x / modifiedSensitivity, deltaPos.y / modifiedSensitivity);
	EventSystem::SendEvent(EVENT_MOUSE_MOVE, &mouseEvent);

	time.TimeStep();

	float dt = (float)time.GetDeltaTimeInSeconds();

// 	if(Input::KeyPressed(keyBindings[WALK_FORWARD_INDEX]))
// 	{		
// 		inputFlags |= WALK_FORWARD;
// 	}
// 
// 	if(Input::KeyReleased(keyBindings[WALK_FORWARD_INDEX]))
// 	{		
// 		inputFlags &= ~(1<<(WALK_FORWARD_INDEX));
// 	}
// 
// 	if(Input::KeyPressed(keyBindings[WALK_BACKWARDS_INDEX]))
// 	{		
// 		inputFlags |= WALK_BACKWARDS;
// 	}
// 
// 	if(Input::KeyReleased(keyBindings[WALK_BACKWARDS_INDEX]))
// 	{
// 		inputFlags &= ~(1<<(WALK_BACKWARDS_INDEX));
// 	}
// 
// 	if(Input::KeyPressed(keyBindings[STRAFE_LEFT_INDEX]))
// 	{
// 		inputFlags |= STRAFE_LEFT;
// 	}
// 
// 	if(Input::KeyReleased(keyBindings[STRAFE_LEFT_INDEX]))
// 	{
// 		inputFlags &= ~(1<<(STRAFE_LEFT_INDEX));
// 	}
// 
// 	if(Input::KeyPressed(keyBindings[STRAFE_RIGHT_INDEX]))
// 	{		
// 		inputFlags |= STRAFE_RIGHT;
// 	}
// 
// 	if(Input::KeyReleased(keyBindings[STRAFE_RIGHT_INDEX]))
// 	{	
// 		inputFlags &= ~(1<<(STRAFE_RIGHT_INDEX));
// 	}
// 
// 	if(Input::KeyPressed(keyBindings[JUMP_INDEX]))
// 	{	
// 		inputFlags |= JUMP_EVENT;
// 	}
// 
// 	if(Input::KeyReleased(keyBindings[JUMP_INDEX]))
// 	{		
// 		inputFlags &= ~(1<<(JUMP_INDEX));
// 	}
// 
// 	if(Input::KeyPressed(keyBindings[SCORE_INDEX]))
// 	{	
// 		inputFlags |= SCORE_EVENT;
// 	}
// 
// 	if(Input::KeyReleased(keyBindings[SCORE_INDEX]))
// 	{		
// 		inputFlags &= ~(1<<(SCORE_INDEX));
// 	}
// 
// 	if(Input::KeyPressed(keyBindings[READY_INDEX]))
// 	{	
// 		inputFlags |= READY_EVENT;
// 	}
// 
// 	if(Input::KeyReleased(keyBindings[READY_INDEX]))
// 	{		
// 		inputFlags &= ~(1<<(READY_INDEX));
// 	}
// 
// 
// 	if(Input::MouseButtonPressed(mouseBindings[0]))
// 	{
// 		mouseDown = true;
// 		inputFlags |= SHOOT_EVENT;
// 	}
// 	else if(Input::MouseButtonReleased(mouseBindings[0]))
// 	{
// 		mouseUp = true;
// 		mouseDown = false;
// 		inputFlags &= ~(1<<(SHOOT_INDEX));
// 	}
// 	
// 	if(Input::MouseButtonPressed(mouseBindings[1]))
// 	{
// 		mouseDown = true;
// 		inputFlags |= ZOOM_EVENT;
// 	}
// 	else if(Input::MouseButtonReleased(mouseBindings[1]))
// 	{
// 		mouseUp = true;
// 		mouseDown = false;
// 		inputFlags &= ~(1<<(ZOOM_INDEX));
// 	}

	if(inputFlags != prevInputFlags)
	{
		inputEvent.SetInputFlags(inputFlags);
		EventSystem::SendEvent(EVENT_SEND_INPUT, &inputEvent);
	}

	prevInputFlags = inputFlags;

	EnterCriticalSection(&criticalSection);
	memcpy(prevMouseButtons, mouseButtons, 8); 
	memcpy(prevKeys, keys, 256);
	LeaveCriticalSection(&criticalSection);
}

void Input::LoadKeyBindings(const char* const _filePath)
{
	ifstream in(_filePath, ios_base::in);

	if(in.is_open())
	{
		char line[256];

		while( in.getline(line,256) )
		{
			istringstream newLine(line, istringstream::in);

			string command;

			newLine >> command;

			char type;
			int key;

			if(command == "MoveForward")
			{				
				newLine >> type >> key;
				if(type == 'K')
					keyBindings.push_back(key);
				else
					mouseBindings.push_back(key);
			}
			else if(command == "MoveBackward")
			{
				newLine >> type >> key;
				if(type == 'K')
					keyBindings.push_back(key);
				else
					mouseBindings.push_back(key);
			}
			else if(command == "StrafeLeft")
			{
				newLine >> type >> key;
				if(type == 'K')
					keyBindings.push_back(key);
				else
					mouseBindings.push_back(key);
			}
			else if(command == "StrafeRight")
			{
				newLine >> type >> key;
				if(type == 'K')
					keyBindings.push_back(key);
				else
					mouseBindings.push_back(key);
			}
			else if(command == "Jump")
			{
				newLine >> type >> key;
				if(type == 'K')
					keyBindings.push_back(key);
				else
					mouseBindings.push_back(key);
			}
			else if(command == "Shoot")
			{
				newLine >> type >> key;
				if(type == 'K')
					keyBindings.push_back(key);
				else
					mouseBindings.push_back(key);
			}
			else if(command == "Zoom")
			{
				newLine >> type >> key;
				if(type == 'K')
					keyBindings.push_back(key);
				else
					mouseBindings.push_back(key);
			}
			else if(command == "PlaySound")
			{
				newLine >> type >> key;
				if(type == 'K')
					keyBindings.push_back(key);
				else
					mouseBindings.push_back(key);
			}
			else if(command == "Score")
			{
				newLine >> type >> key;
				if(type == 'K')
					keyBindings.push_back(key);
				else
					mouseBindings.push_back(key);
			}
			else if(command == "Ready")
			{
				newLine >> type >> key;
				if(type == 'K')
					keyBindings.push_back(key);
				else
					mouseBindings.push_back(key);
			}
			else if(command == "Sensitivity")
			{
				newLine >> sensitivity;
			}
			else if(command == "Inverted")
			{
				newLine >> inverted;
			}
		}

		in.close();
	}
}

void Input::LoadSaveControls( const char* const _filePath )
{
	ifstream in(_filePath, ios_base::in);

	if(in.is_open())
	{
		char line[256];

		while( in.getline(line,256) )
		{
			istringstream newLine(line, istringstream::in);

			string command;

			newLine >> command;

			if(command == "inverted")
			{
				newLine >> inverted;
			}
			else if(command == "sensitivity")
			{
				newLine >> sensitivity;
			}
		}

		in.close();
	}
}
