#include "DirectInput.h"

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
	
}

bool DirectInput::Frame()
{
	return true;
}

bool DirectInput::ReadeKeyboard()
{
	return true;
}

bool DirectInput::ReadMouse()
{
	return true;
}

void DirectInput::ProcessInput()
{

}

bool DirectInput::IsEscapePressed()
{
	return true;
}

void DirectInput::GetMouseLocation(int& _xPos, int& _yPos)
{

}

