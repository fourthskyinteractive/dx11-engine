#ifndef DIRECTINPUT_H_
#define DIRECTINPUT_H_

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class DirectInput
{
public:
	DirectInput();
	DirectInput(const DirectInput& _dInput);
	~DirectInput();

	bool Initialize(HINSTANCE _hinstance, HWND _hwnd, int _screenWidth, int _screenHeight);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	void GetMouseLocation(int& _xPos, int& _yPos);

private:
	bool ReadeKeyboard();
	bool ReadMouse();
	void ProcessInput();

	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	unsigned char keyboardState[256];
	DIMOUSESTATE mouseState;

	int screenWidth, screenHeight;
	int mouseX, mouseY;
};
#endif