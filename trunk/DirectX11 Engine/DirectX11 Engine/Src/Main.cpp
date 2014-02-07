#include <windows.h>
#include <process.h>

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <ctime>

#include <stdio.h>
#include <DbgHelp.h>
#pragma comment (lib, "dbghelp.lib")

// 
//  #ifdef _DEBUG
//  #include <vld.h>
//  #endif

//temp - remove
// #include "Renderer\D3D9Renderer.h"
 #include "Game\Game.h"
 #include "Game\Definitions.h"

using std::cout;
using std::ifstream;
using std::string;
using std::istringstream;

// Let's get a command console for printing debug information when running DEBUG
#ifdef _DEBUG
#include "Utility\Misc\DebugOutput.h"
#pragma comment(linker, "/SUBSYSTEM:Console")
#else
#pragma comment(linker, "/SUBSYSTEM:Windows")
#endif

HCURSOR hCursor =  LoadCursorFromFile(TEXT("Res/Windows/cursor_arrow.ani"));

// Variables containing important window/graphics related information
int screenWidth;
int screenHeight;
bool vsync;
bool fullscreen;
float gameGamma = 0;
int brightness = 0;

LPCSTR applicationName = "Engine";
HINSTANCE hInstance;
HWND hwnd;


LONG WINAPI CatchAll(_EXCEPTION_POINTERS *pExceptionInfo);
/*
Handle OS/System messages
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
	case WM_CREATE: 
		{
// 			RAWINPUTDEVICE Rid[2];
// 			Rid[0].usUsagePage = 0x01; 
// 			Rid[0].usUsage = 0x02; 
// 			Rid[0].dwFlags = RIDEV_NOLEGACY;
// 			Rid[0].hwndTarget = 0;
// 			Rid[1].usUsagePage = 0x01; 
// 			Rid[1].usUsage = 0x06; 
// 			Rid[1].dwFlags = RIDEV_NOLEGACY;
// 			Rid[1].hwndTarget = 0;
// 
// 			if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
// 			{
// 				LPTSTR errorText = NULL;
// 				DWORD errorCode = GetLastError();
// 				FormatMessage(
// 					FORMAT_MESSAGE_FROM_SYSTEM
// 					|FORMAT_MESSAGE_ALLOCATE_BUFFER
// 					|FORMAT_MESSAGE_IGNORE_INSERTS,  
// 					NULL,
// 					errorCode,
// 					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
// 					(LPTSTR)&errorText, 
// 					0,
// 					NULL);
// 
// 				MessageBox(HWND_DESKTOP, errorText,
// 					"Raw Input Error", MB_OK | MB_ICONERROR);
// 
// 				return -1;
// 			}
// 
// 			SetCursor( hCursor );
// 
// 			return(0);
		}
		break;

	case WM_INPUT:
		{
// 			RAWINPUT ri;
// 			UINT dwSize = sizeof(RAWINPUT);
// 
// 			GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &ri, &dwSize, sizeof(RAWINPUTHEADER));
// 
// 			if (ri.header.dwType == RIM_TYPEKEYBOARD)
// 			{
// 				//Input::RawKeyInput(ri.data.keyboard);
// 			}
// 			else if (ri.header.dwType == RIM_TYPEMOUSE) 
// 				//Input::RawMouseInput(ri.data.mouse);		
// 
// 			return 0;
		}
		break;

	case WM_ACTIVATE:
		{
			//	gaining focus
			if (LOWORD(wparam) != WA_INACTIVE)
			{
// 				RECT windowRect;
// 				GetWindowRect(hwnd,&windowRect);
// 				windowRect.left		+= 10;
// 				windowRect.right	-= 10;
// 				windowRect.bottom	-= 10;
// 				windowRect.top		+= 10;
// 				ClipCursor(&windowRect);
			}
			else // losing focus
			{
				ClipCursor(NULL);
			}
		}
		break;

	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		break;

	case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}
		break;

	default:
		{
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
		break;
	}
	return 1;
}

/*
We want to initialize all application specific data here
*/
void InitializeApplication()
{
	srand((unsigned int)time(0));
	//Load from .ini all configuration data
	int baseScreenWidth = ScreenWidth;
	int baseScreenHeight = ScreenHeight;

	char line[256];

	string filePath = "Res/Data/Game/VideoSettings.txt";
	int fov = 0;
	bool isFullScreen = false;
	std::ifstream file;
	file.clear();
	file.open(filePath, std::ios_base::in);
	if (file.is_open())
	{
		//Brightness
		int videoSettings = 0;
		file >> videoSettings;
		brightness = videoSettings;
		file.ignore(INT_MAX, '\t');

		//Gamma
		float gamma = 0.0f;
		file >> gamma;
		gameGamma = gamma;
		file.ignore(INT_MAX, '\t');

		//Resolution - Width
		videoSettings = 0;
		file >> videoSettings;
		videoSettings = ScreenWidth;
		file.ignore(INT_MAX, '\t');

		//Resolution - Height
		videoSettings = 0;
		file >> videoSettings;
		videoSettings = ScreenHeight;
		file.ignore(INT_MAX, '\t');

		//Fov
		videoSettings = 0;
		file >> videoSettings;
		fov = videoSettings;
		file.ignore(INT_MAX, '\t');

		//Fullscreen
		bool fScreen = 0;
		file >> fScreen;
		isFullScreen = fScreen;
	}

	ifstream startup;

	startup.open("Res/Data/startup.ini");

	while( startup.getline(line,256) )
	{
		istringstream newLine(line, istringstream::in);

		string command;

		newLine >> command;

		if(command == "fullscreen")
			newLine >> fullscreen;
		else if(command == "vsync")
			newLine >> vsync;
		else if(command == "screen_width_fullscreen")
		{
			if(fullscreen)
				newLine >> screenWidth;
		}
		else if(command == "screen_height_fullscreen")
		{
			if(fullscreen)
				newLine >> screenHeight;
		}
		else if(command == "screen_width_windowed")
		{
			if(!fullscreen)
				newLine >> screenWidth;
		}
		else if(command == "screen_height_windowed")
		{
			if(!fullscreen)
				newLine >> screenHeight;
		}
		else if(command == "fov")
		{
			newLine >> ScreenInformation::fov;
		}
	}


	std::string filePath2 = "Res/Data/Game/VideoSettings.txt";

	std::ifstream file2;
	file2.clear();
	file2.open(filePath2, std::ios_base::in);
	if (file2.is_open())
	{
		//Brightness
		int videoSettings = 0;
		file2 >> videoSettings;
		file2.ignore(INT_MAX, '\t');

		//Gamma
		float gamma = 0.0f;
		file2 >> gamma;
		file2.ignore(INT_MAX, '\t');

		//Resolution - Width
		videoSettings = 0;
		file2 >> videoSettings;
		screenWidth = videoSettings;
		file2.ignore(INT_MAX, '\t');

		//Resolution - Height
		videoSettings = 0;
		file2 >> videoSettings;
		screenHeight = videoSettings;
		file2.ignore(INT_MAX, '\t');
	}


	//Setting full screen to the value saved in the video settings
	ScreenInformation::fullscreen = isFullScreen;
	fullscreen = isFullScreen;
	//Setting fov to the value saved in the video settings
	ScreenInformation::fov = (float)fov;

	//Setting gamma and brightness to the value saved in the video settings
	//VideoSettings::gameGamma = gameGamma;
	//VideoSettings::gameBrigthness = brightness;


	//Get the % increase or decrease
	float horizontalTemp = (float)screenWidth/baseScreenWidth;
	float verticalTemp = (float)screenHeight/baseScreenHeight;
	ScreenInformation::screenHorizontal = screenWidth;
	ScreenInformation::screenVertical = screenHeight;

	ScreenInformation::orthoRatio = 1.2f;

	//Get the difference from the base one
	ScreenInformation::horizontalDifference = horizontalTemp - 1.0f;
	ScreenInformation::verticalDifference = verticalTemp - 1.0f;

	ScreenInformation::horizontalRatio = ScreenInformation::horizontalDifference/2;
	ScreenInformation::verticalRatio = ScreenInformation::verticalDifference/2;
	ScreenInformation::aspectRatio = ((float)screenWidth) / ((float)screenHeight);

// 	//Get rid of sticky keys
// 	STICKYKEYS StartupStickyKeys = {sizeof(STICKYKEYS), 0};
// 	SystemParametersInfo(SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), &StartupStickyKeys, 0);
// 
// 	STICKYKEYS skOff = StartupStickyKeys;
// 	if( (skOff.dwFlags & SKF_STICKYKEYSON) == 0 )
// 	{
// 		skOff.dwFlags &= ~SKF_HOTKEYACTIVE;
// 		skOff.dwFlags &= ~SKF_CONFIRMHOTKEY;
// 
// 		SystemParametersInfo(SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), &skOff, 0);
// 	}
}

void InitializeWindow()
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY, windowWidth, windowHeight;

	DWORD dwWindowStyleFlags = WS_VISIBLE;

	dwWindowStyleFlags |= WS_CAPTION | WS_SYSMENU;

	hInstance = GetModuleHandle(NULL);

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon		 = LoadIcon(hInstance, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth = ScreenWidth;
	screenHeight = ScreenHeight;

	if(fullscreen)
	{
		dwWindowStyleFlags |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		windowWidth = screenWidth;
		windowHeight = screenHeight;

		posX = posY = 0;
	}
	else
	{
		dwWindowStyleFlags |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

		RECT rWindow;
		rWindow.left	= 0;
		rWindow.top		= 0;
		rWindow.right	= screenWidth;
		rWindow.bottom	= screenHeight;

		AdjustWindowRectEx(&rWindow, dwWindowStyleFlags, FALSE, WS_EX_APPWINDOW);

		windowWidth		= rWindow.right - rWindow.left;
		windowHeight	= rWindow.bottom - rWindow.top;
	}

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName, dwWindowStyleFlags,
		posX, posY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{

	SetUnhandledExceptionFilter(CatchAll);

	MSG		msg;

	if ( hPrevInstance ) {
		return -1;
	}

	InitializeApplication();

	InitializeWindow();

	bool result = Game::Initialize(hInstance, hwnd, fullscreen, vsync, screenWidth, screenHeight);

	if(!result)
	{
		return -1;
	}

	//VideoSettingsMenu::GetInstance()->SetBrightness(NULL, brightness);
	//VideoSettingsMenu::GetInstance()->SetGamma(gameGamma);

	ZeroMemory(&msg, sizeof(MSG));

	while(Game::isRunning)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if(msg.message == WM_QUIT)
			{
				Game::isRunning = false;
			}
			if(msg.message == WM_SIZE)
			{
				int width = (msg.lParam & 255);
				int height = (msg.lParam & 65280);
			}
		}
		else
		{
			Game::Run();
		}		
	}

	Game::Exit();

	ShowCursor(true);

	return 0;
}

int main()
{
#ifdef _DEBUG
	DebugOutput::Initialize(DebugOutput::RENDER_MSG | DebugOutput::FBXLOADER_MSG);
#endif

	int msg = WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOWDEFAULT);

	return 0;
}

LONG WINAPI CatchAll(_EXCEPTION_POINTERS *pExceptionInfo)
{
	int index = 0;
	LPCTSTR filename = "dumpfile.mdmp";
	HANDLE hFile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL );

	char *report = NULL;
	if (hFile!=INVALID_HANDLE_VALUE)
	{
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
		report = new char[1024];
		sprintf_s(report, 1024, "Look in dump file: dumpfile.mdmp\nThe error was:\n\n%d\n%s", ExInfo.ExceptionPointers->ExceptionRecord->ExceptionCode, ExInfo.ExceptionPointers->ExceptionRecord->ExceptionRecord->ExceptionInformation);
		MessageBox(NULL, report, "AHHH BREAK!",MB_OK);
		CloseHandle(hFile);
		delete[] report;
	}

	return 0;

}