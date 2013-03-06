#include "DirectInput.h"
#include "../Game/Definitions.h"

DirectInput::DirectInput()
{
	directInput = NULL;
	keyboard = NULL;
	mouse = NULL;
}

DirectInput::DirectInput(const DirectInput& _dInput)
{
	
}

DirectInput::~DirectInput()
{

}

bool DirectInput::Initialize(HINSTANCE _hinstance, HWND _hwnd, int _screenWidth, int _screenHeight)
{
	HRESULT hr;

	//Store the screen size which will be used for positioning the mouse cursor
	screenWidth = _screenWidth;
	screenHeight = _screenHeight;

	//Initialize the location of the mouse on the screen
	mouseX = 0;
	mouseY = 0;

	//Initialize the main direct input interface
	hr = DirectInput8Create(_hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	if(FAILED(hr))
	{
		return false;
	}

	//Initialize the dsirect input interface for the keyboard
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	if(FAILED(hr))
	{
		return false;
	}

	//Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		return false;
	}

	//Set the cooperative level of the keyboard to not share with other programs
	//If you want to share with other programs just change the flag from DISCL_EXCLUSIVE to DISCL_NONEXCLUSIVE
	hr = keyboard->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(hr))
	{
		return false;
	}

	//Now acquire the keyboard
	hr = keyboard->Acquire();
	if(FAILED(hr))
	{
		return false;
	}

	//Initialize the direct input interface for the mouse
	hr = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	if(FAILED(hr))
	{
		return false;
	}
	
	//Set the data format for the mouse using the pre-defined mouse data format
	hr = mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(hr))
	{
		return false;
	}

	//Set the cooperative level of the mouse to share with other programs
	hr = mouse->SetCooperativeLevel(_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(hr))
	{
		return false;
	}

	//Acqure the mouse
	hr = mouse->Acquire();
	if(FAILED(hr))
	{
		return false;
	}

	return true;
}

void DirectInput::Shutdown()
{
	//Release the mouse
	if(mouse)
	{
		mouse->Unacquire();
		mouse->Release();
		mouse = 0;
	}
	
	//Release the keyboard
	if(keyboard)
	{
		keyboard->Unacquire();
		keyboard->Release();
		keyboard = 0;
	}
	if(directInput)
	{
		directInput->Release();
		directInput = 0;
	}

	return;
}

bool DirectInput::Update()
{
	bool result;

	//Read the current state of the keyboard
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}
	
	//Raad the current state of the mouse
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	//Process the changes in the mouse and keyboard
	ProcessInput();

	return true;
}

bool DirectInput::ReadKeyboard()
{
	HRESULT hr;

	//Read the keyboard device
	ZeroMemory(&keyboardState, sizeof(keyboardState));
	hr = keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
	if(FAILED(hr))
	{
		//If the keyboard lost focus or was not acquired then try to get control back
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool DirectInput::ReadMouse()
{
	HRESULT hr;

	//Read the mouse device
	ZeroMemory(&mouseState, sizeof(mouseState));
	hr = mouse->GetDeviceState(sizeof(mouseState), (LPVOID)&mouseState);
	if(FAILED(hr))
	{
		//If the keyboard lost focus or was not acquired then try to get control back
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool DirectInput::IsKeyPressed(int _key)
{
	if(keyboardState[_key] & 0x80)
	{
		return true;
	}

	return false;
}

bool DirectInput::IsMouseButtonPressed(int _button)
{
	if(mouseState.rgbButtons[_button & 0x80])
	{
		return true;
	}

	return false;
}

void DirectInput::ProcessInput()
{
	//Update the location of the mouse cursor based on the change of the mouse location during the frame
	mouseX += mouseState.lX;
	mouseY += mouseState.lY;

	//Ensure the mouse location doesn't exceed the creen width or height
	//if(mouseX < 0) { mouseX = 0; }
	//if(mouseY < 0) { mouseY = 0; }

	//if(mouseX > screenWidth)  { mouseX = screenWidth; }
	//if(mouseY > screenHeight) { mouseY = screenHeight; }

	return;
}

bool DirectInput::IsEscapePressed()
{
	//Do a bitwise "and" on the keyboard state to check if the escape key is currently being pressed
	if(keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

void DirectInput::GetMouseLocation(int& _xPos, int& _yPos)
{
	_xPos = mouseX;
	_yPos = mouseY;
}