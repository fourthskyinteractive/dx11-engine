#ifndef INPUT_H
#define INPUT_H

#include <Windows.h>
#include <process.h>
#include <vector>
using std::vector;

#include "../EventSystem/EventSystem.h"
#include "../Utility/Misc/Timer.h"

struct Key
{
	Key() : down(false) {};
	bool down;
};

class Input
{
public:
	Input(){};
	Input(const Input&){};
	~Input(){DeleteCriticalSection(&criticalSection);};

	static bool Init();
	static void ClearKeys();
	static void RawKeyInput(RAWKEYBOARD& kb);
	static void RawMouseInput(RAWMOUSE& mouse);

	static bool KeyDown(char _keyCode);
	static bool KeyPressed(char _keyCode);
	static bool KeyReleased(char _keyCode);

	static bool MouseButtonPressed(char _button);
	static bool MouseButtonReleased(char _button);

	static void Update();
	static POINT GetCursorPosition(){return cursorPos;}

	static void CheckSpecialCases();
	static bool GetMouseUp() {return mouseUp;}
	static bool GetMouseDown() {return (mouseDown && !prevMouseDown);}

	static char GetKeyPressed();

	static void SetSensitivity(float val) { sensitivity = val; }
	static void SetInverted(bool val) { inverted = val; }
private:
	static Key keys[256];
	static Key prevKeys[256];

	static Key mouseButtons[8];
	static Key prevMouseButtons[8];
	static CRITICAL_SECTION criticalSection;

	static float sensitivity;
	static bool inverted;
	static vector<char> keyBindings;
	static vector<char> mouseBindings;

	static void LoadKeyBindings(const char* const _filePath);
	static void LoadSaveControls(const char* const _filePath);
	
	static short inputFlags;
	
	static bool prevMouseDown;
	static bool mouseUp;
	static bool mouseDown;

	static POINT cursorPos;
	static POINT cursorRelPos;
	static POINT prevCursorPos;

	static InputEvent inputEvent;
	static MouseEvent mouseEvent;

	//Timer class
	static Timer time;
	static float limit;
	static float step;


};

#endif