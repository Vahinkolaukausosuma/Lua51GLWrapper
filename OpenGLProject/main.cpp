#define WINDOWS_IGNORE_PACKING_MISMATCH // XD
#include <GL/glew.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL.h>

#include <iostream>
#include <string.h>
#include <InitGuid.h>
#include <Windows.h>
//#include <string>
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <vector>
#include <xinput.h>
//extern "C"
//{
//#include "lua51/include/lua.h"
//#include "lua51/include/lauxlib.h"
//#include "lua51/include/lualib.h"
//}
//#pragma comment(lib, "lua51/lua5.1.lib")


extern "C"
{
#include "luajit210b3/include/lua.h"
#include "luajit210b3/include/lauxlib.h"
#include "luajit210b3/include/lualib.h"
}
#pragma comment(lib, "luajit210b3/lua51.lib")


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

POINT p;
SDL_Window* WindowTable[50];
SDL_GLContext ContextTable[50];
int WindowPointer = 0;
bool m_IsClosed = false;
bool firstScreenCap = true;
int ScreenX = 0;
int ScreenY = 0;
BYTE* ScreenData = 0;
HDC hScreen = 0;
HDC hdcMem = 0;
HBITMAP hBitmap = 0;
HGDIOBJ hOld = 0;
BITMAPINFOHEADER bmi = { 0 };
BOOL CALLBACK EnumFFDevicesCallback(const DIDEVICEINSTANCE* pInst, VOID* pContext);
BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);
VOID FreeDirectInput();
HRESULT SetDeviceForcesXY();
LPDIRECTINPUT8          g_pDI = nullptr;
LPDIRECTINPUTDEVICE8    g_pDevice = nullptr;
LPDIRECTINPUTEFFECT     g_pEffect = nullptr;
BOOL                    g_bActive = TRUE;
DWORD                   g_dwNumForceFeedbackAxis = 0;
INT                     g_nXForce = 0;
INT                     g_nYForce = 0;
HWND					HWNDProgram = nullptr;



#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=nullptr; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }
//-----------------------------------------------------------------------------
// Name: InitDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
std::string formatError(HRESULT hr) {
	switch (hr) {
	case DI_OK:                     return "DI_OK";
	case DIERR_INVALIDPARAM:        return "DIERR_INVALIDPARAM";
	case DIERR_NOTINITIALIZED:      return "DIERR_NOTINITIALIZED";
	case DIERR_ALREADYINITIALIZED:  return "DIERR_ALREADYINITIALIZED";
	case DIERR_INPUTLOST:           return "DIERR_INPUTLOST";
	case DIERR_ACQUIRED:            return "DIERR_ACQUIRED";
	case DIERR_NOTACQUIRED:         return "DIERR_NOTACQUIRED";
	case E_HANDLE:                  return "E_HANDLE";
	case DIERR_DEVICEFULL:          return "DIERR_DEVICEFULL";
	case DIERR_DEVICENOTREG:        return "DIERR_DEVICENOTREG";
	default:                        return "UNKNOWN";
	}
}



HRESULT InitDirectInput()
{
	DIPROPDWORD dipdw;
	HRESULT hr;

	//g_pDevice->Unacquire();

	// Register with the DirectInput subsystem and get a pointer
	// to a IDirectInput interface we can use.
	printf("if (FAILED(hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,\n");
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&g_pDI, nullptr)))
	{
		return hr;
	}

	// Look for a force feedback device we can use
	printf("if (FAILED(hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL,\n");
	if (FAILED(hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL,
		EnumFFDevicesCallback, nullptr,
		DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK)))
	{
		return hr;
	}
	printf("if (!g_pDevice)\n");
	if (!g_pDevice)
	{
		printf("[C++] No DI Device !g_pDevice Line 112\n");
		return 420;
	}

	// Set the data format to "simple joystick" - a predefined data format. A
	// data format specifies which controls on a device we are interested in,
	// and how they should be reported.
	//
	// This tells DirectInput that we will be passing a DIJOYSTATE structure to
	// IDirectInputDevice8::GetDeviceState(). Even though we won't actually do
	// it in this sample. But setting the data format is important so that the
	// DIJOFS_* values work properly.
	printf("if (FAILED(hr = g_pDevice->SetDataFormat(&c_dfDIJoystick)))\n");
	if (FAILED(hr = g_pDevice->SetDataFormat(&c_dfDIJoystick)))
		return hr;

	// Set the cooperative level to let DInput know how this device should
	// interact with the system and with other DInput applications.
	// Exclusive access is required in order to perform force feedback.
	printf("if (FAILED(hr = g_pDevice->SetCooperativeLevel(HWNDProgram, DISCL_EXCLUSIVE | DISCL_FOREGROUND)))\n");
	if (FAILED(hr = g_pDevice->SetCooperativeLevel(HWNDProgram, DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
	{
		return hr;
	}

	// Since we will be playing force feedback effects, we should disable the
	// auto-centering spring.
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = FALSE;
	printf("if (FAILED(hr = g_pDevice->SetProperty(DIPROP_AUTOCENTER, &dipdw.diph)))\n");
	if (FAILED(hr = g_pDevice->SetProperty(DIPROP_AUTOCENTER, &dipdw.diph)))
		return hr;
	printf("if (FAILED(hr = g_pDevice->EnumObjects(EnumAxesCallback,\n");
	// Enumerate and count the axes of the joystick 
	if (FAILED(hr = g_pDevice->EnumObjects(EnumAxesCallback,
		(VOID*)&g_dwNumForceFeedbackAxis, DIDFT_AXIS)))
		return hr;

	// This simple sample only supports one or two axis joysticks
	g_dwNumForceFeedbackAxis = 1;


	//if (g_dwNumForceFeedbackAxis > 2)
	//	g_dwNumForceFeedbackAxis = 2;

	// This application needs only one effect: Applying raw forces.
	DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
	LONG rglDirection[2] = { 0, 0 };
	DICONSTANTFORCE cf = { 0 };

	DIEFFECT eff;
	ZeroMemory(&eff, sizeof(eff));
	eff.dwSize = sizeof(DIEFFECT);
	eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	eff.dwDuration = INFINITE;
	eff.dwSamplePeriod = 0;
	eff.dwGain = DI_FFNOMINALMAX;
	eff.dwTriggerButton = DIEB_NOTRIGGER;
	eff.dwTriggerRepeatInterval = 0;
	eff.cAxes = g_dwNumForceFeedbackAxis;
	eff.rgdwAxes = rgdwAxes;
	eff.rglDirection = rglDirection;
	eff.lpEnvelope = 0;
	eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
	eff.lpvTypeSpecificParams = &cf;
	eff.dwStartDelay = 0;
	printf("if (FAILED(hr = g_pDevice->CreateEffect(GUID_ConstantForce,\n");
	// Create the prepared effect
	if (FAILED(hr = g_pDevice->CreateEffect(GUID_ConstantForce,
		&eff, &g_pEffect, nullptr)))
	{
		return hr;
	}

	if (!g_pEffect)
		return E_FAIL;

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: EnumAxesCallback()
// Desc: Callback function for enumerating the axes on a joystick and counting
//       each force feedback enabled axis
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumAxesCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,
	VOID* pContext)
{
	auto pdwNumForceFeedbackAxis = reinterpret_cast<DWORD*>(pContext);

	if ((pdidoi->dwFlags & DIDOI_FFACTUATOR) != 0)
		(*pdwNumForceFeedbackAxis)++;

	return DIENUM_CONTINUE;
}




//-----------------------------------------------------------------------------
// Name: EnumFFDevicesCallback()
// Desc: Called once for each enumerated force feedback device. If we find
//       one, create a device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK EnumFFDevicesCallback(const DIDEVICEINSTANCE* pInst,
	VOID* pContext)
{
	LPDIRECTINPUTDEVICE8 pDevice;
	HRESULT hr;

	// Obtain an interface to the enumerated force feedback device.
	hr = g_pDI->CreateDevice(pInst->guidInstance, &pDevice, nullptr);

	// If it failed, then we can't use this device for some
	// bizarre reason.  (Maybe the user unplugged it while we
	// were in the middle of enumerating it.)  So continue enumerating
	if (FAILED(hr))
		return DIENUM_CONTINUE;

	// We successfully created an IDirectInputDevice8.  So stop looking 
	// for another one.
	g_pDevice = pDevice;
	pDevice = nullptr;

	return DIENUM_STOP;
}

//-----------------------------------------------------------------------------
// Name: FreeDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
VOID FreeDirectInput()
{
	// Unacquire the device one last time just in case 
	// the app tried to exit while the device is still acquired.
	if (g_pDevice)
		g_pDevice->Unacquire();

	// Release any DirectInput objects.
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(g_pDevice);
	SAFE_RELEASE(g_pDI);
}



//-----------------------------------------------------------------------------
// Name: SetDeviceForcesXY()
// Desc: Apply the X and Y forces to the effect we prepared.
//-----------------------------------------------------------------------------
HRESULT SetDeviceForcesXY()
{
	// Modifying an effect is basically the same as creating a new one, except
	// you need only specify the parameters you are modifying
	LONG rglDirection;
	DICONSTANTFORCE cf;


	cf.lMagnitude = g_nXForce;
	rglDirection = 0;

	//else
	//{
	//	// If two force feedback axis, then apply magnitude from both directions 
	//	rglDirection[0] = g_nXForce;
	//	rglDirection[1] = g_nYForce;
	//	cf.lMagnitude = (DWORD)sqrt((double)g_nXForce * (double)g_nXForce +
	//		(double)g_nYForce * (double)g_nYForce);
	//}

	//printf("[C++] g_dwNumForceFeedbackAxis %d\n", g_dwNumForceFeedbackAxis);
	//HRESULT hr;
	//if (FAILED(hr = InitDirectInput()))
	//{
	//	printf("[C++] InitDirectInput failed Errorcode: %d, Error message: %s\n", hr, formatError(hr).c_str());
	//}
	DIEFFECT eff;
	ZeroMemory(&eff, sizeof(eff));
	eff.dwSize = sizeof(DIEFFECT);
	eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	eff.cAxes = g_dwNumForceFeedbackAxis;
	eff.rglDirection = &rglDirection;
	eff.lpEnvelope = 0;
	eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
	eff.lpvTypeSpecificParams = &cf;
	eff.dwStartDelay = 0;
	//std::cout << g_dwNumForceFeedbackAxis << std::endl;
	// Now set the new parameters and start the effect immediately.
	return g_pEffect->SetParameters(&eff, DIEP_DIRECTION |
		DIEP_TYPESPECIFICPARAMS |
		DIEP_START);
}


int main(int argc, char* argv[])
{
	return 1;
}

bool MakeWindowTransparent(SDL_Window* window, COLORREF colorKey) {
	// Get window handle (https://stackoverflow.com/a/24118145/3357935)
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);  // Initialize wmInfo
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hWnd = wmInfo.info.win.window;

	// Change window type to layered (https://stackoverflow.com/a/3970218/3357935)
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	// Set transparency color
	return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}

int Lua_keybd_event(lua_State* L)
{
	keybd_event((BYTE)lua_tointeger(L, -2), MapVirtualKey((BYTE)lua_tointeger(L, -2), 0), KEYEVENTF_EXTENDEDKEY, 0);
	Sleep(lua_tointeger(L, -1));
	keybd_event((BYTE)lua_tointeger(L, -2), MapVirtualKey((BYTE)lua_tointeger(L, -2), 0), KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	return 1;
}
int Lua_SendClick(lua_State* L)
{
	INPUT Inputs[3] = { 0 };

	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dx = lua_tointeger(L, -2); // desired X coordinate
	Inputs[0].mi.dy = lua_tointeger(L, -1); // desired Y coordinate
	Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	Inputs[1].type = INPUT_MOUSE;
	Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	Inputs[2].type = INPUT_MOUSE;
	Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	SendInput(3, Inputs, sizeof(INPUT));
	return 0;
}
int Lua_IsKeyDown(lua_State* L)
{
	if ((GetKeyState(lua_tointeger(L, -1)) & 0x8000) != 0)
	{
		lua_pushboolean(L, true);
	}
	else
	{
		lua_pushboolean(L, false);
	}
	return 1;
}


int CreateDisplay(lua_State* L)
{
	int width = (int)lua_tonumber(L, -4);
	int height = (int)lua_tonumber(L, -3);
	const char* Title = lua_tostring(L, -2);
	Uint32 flags = (int)lua_tonumber(L, -1);

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	WindowTable[WindowPointer] = SDL_CreateWindow(Title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	ContextTable[WindowPointer] = SDL_GL_CreateContext(WindowTable[WindowPointer]);

	GLenum status = glewInit();

	m_IsClosed = false;
	return 0;
}


struct MonitorRects
{
	std::vector<RECT>   rcMonitors;

	static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
	{
		MonitorRects* pThis = reinterpret_cast<MonitorRects*>(pData);
		pThis->rcMonitors.push_back(*lprcMonitor);
		return TRUE;
	}

	MonitorRects()
	{
		EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
	}
};

int ScreenCap_WholeScreen(lua_State* L)
{

	if (firstScreenCap)
	{
		
		hScreen = GetDC(NULL);
		ScreenX = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		//ScreenX = 3200;
		ScreenY = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		//ScreenY = 1189;
		ScreenData = (BYTE*)malloc((5 * ScreenX * ScreenY));
		hdcMem = CreateCompatibleDC(hScreen);
		hBitmap = CreateCompatibleBitmap(hScreen, ScreenX, ScreenY);
		hOld = SelectObject(hdcMem, hBitmap);
		hdcMem = CreateCompatibleDC(hScreen);
		hBitmap = CreateCompatibleBitmap(hScreen, ScreenX, ScreenY);
		bmi.biSize = sizeof(BITMAPINFOHEADER);
		bmi.biPlanes = 1;
		bmi.biBitCount = 32;
		bmi.biWidth = ScreenX;
		bmi.biHeight = -ScreenY;
		bmi.biCompression = BI_RGB;
		bmi.biSizeImage = 0;
		firstScreenCap = false;
		//system("pause");
		//printf("%d %d %d\n", ScreenX, -ScreenY, ScreenX * ScreenY);
	}
	
	
	
	hOld = SelectObject(hdcMem, hBitmap);
	//BitBlt(hdcMem, 0, 0, 3200, 1189, hScreen, 0, 0, SRCCOPY);
	//BitBlt()
	BitBlt(hdcMem, 0, 0, ScreenX, ScreenY, hScreen, -1280, 0, SRCCOPY);
	SelectObject(hdcMem, hOld);


	//printf("w=%d, h=%d\n", ScreenX , ScreenY);
	//if (ScreenData)
		//free(ScreenData);
	

	GetDIBits(hdcMem, hBitmap, 0, ScreenY, ScreenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

	//ReleaseDC(GetDesktopWindow(), hScreen);
	//DeleteDC(hdcMem);
	//DeleteObject(hBitmap);
	return 0;
}

int ScreenCap(lua_State* L)
{
	if (firstScreenCap)
	{

		hScreen = GetDC(GetDesktopWindow());
		ScreenX = GetSystemMetrics(SM_CXSCREEN);
		ScreenY = GetSystemMetrics(SM_CYSCREEN);
		ScreenData = (BYTE*)malloc(4 * ScreenX * ScreenY);
		hdcMem = CreateCompatibleDC(hScreen);
		hBitmap = CreateCompatibleBitmap(hScreen, ScreenX, ScreenY);
		hOld = SelectObject(hdcMem, hBitmap);
		hdcMem = CreateCompatibleDC(hScreen);
		hBitmap = CreateCompatibleBitmap(hScreen, ScreenX, ScreenY);
		bmi.biSize = sizeof(BITMAPINFOHEADER);
		bmi.biPlanes = 1;
		bmi.biBitCount = 32;
		bmi.biWidth = ScreenX;
		bmi.biHeight = -ScreenY;
		bmi.biCompression = BI_RGB;
		bmi.biSizeImage = 0;
		firstScreenCap = false;
		//system("pause");
		printf("%d %d\n", ScreenX, ScreenY);
	}




	hOld = SelectObject(hdcMem, hBitmap);
	BitBlt(hdcMem, 0, 0, ScreenX, ScreenY, hScreen, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hOld);


	//printf("w=%d, h=%d\n", ScreenX , ScreenY);
	//if (ScreenData)
		//free(ScreenData);


	GetDIBits(hdcMem, hBitmap, 0, ScreenY, ScreenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

	//ReleaseDC(GetDesktopWindow(), hScreen);
	//DeleteDC(hdcMem);
	//DeleteObject(hBitmap);
	return 0;
}

int ReadPixelColor(lua_State* L)
{
	int x = (int)lua_tonumber(L, -2);
	int y = (int)lua_tonumber(L, -1);
	//printf("x=%d, y=%d\n",x,y);
	
	if (x >= 0 && x <= ScreenX && y >= 0 && y <= ScreenY && x*y < ScreenX*ScreenY)
	{
		lua_pushinteger(L, ScreenData[4 * ((y * ScreenX) + x) + 2]);
		lua_pushinteger(L, ScreenData[4 * ((y * ScreenX) + x) + 1]);
		lua_pushinteger(L, ScreenData[4 * ((y * ScreenX) + x)]);
		return 3;
	}
	lua_pushinteger(L, 420); lua_pushinteger(L, 420); lua_pushinteger(L, 420);
	return 3;
}
int ReadPixelColorUnsafeAsFuck(lua_State* L)
{
	int x = (int)lua_tonumber(L, -2);
	int y = (int)lua_tonumber(L, -1);

	lua_pushinteger(L, ScreenData[4 * ((y * ScreenX) + x) + 2]);
	lua_pushinteger(L, ScreenData[4 * ((y * ScreenX) + x) + 1]);
	lua_pushinteger(L, ScreenData[4 * ((y * ScreenX) + x)]);
	return 3;
}

int MoveWindow(lua_State* L)
{
	int x = (int)lua_tonumber(L, -2);
	int y = (int)lua_tonumber(L, -1);

	SDL_SetWindowPosition(WindowTable[WindowPointer], x, y);
	return 0;
}

int destroyDisplay(lua_State* L)
{
	SDL_GL_DeleteContext(ContextTable[WindowPointer]);
	SDL_DestroyWindow(WindowTable[WindowPointer]);
	SDL_Quit();
	return 0;
}

int Lua_glBegLines(lua_State* L)
{
	glBegin(GL_LINES);
	return 0;
}
int Lua_glBegPoints(lua_State* L)
{
	glBegin(GL_POINTS);
	return 0;
}
int Lua_glEnd(lua_State* L)
{
	glEnd();
	return 0;
}
int displayClear(lua_State* L)
{
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	return 0;
}
int displayClearNoAlpha(lua_State* L)
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_STENCIL_BUFFER_BIT);
	glClear(GL_ACCUM_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	return 0;
}

int Lua_displayUpdate(lua_State* L)
{
	SDL_GL_SwapWindow(WindowTable[WindowPointer]);
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT) {
			m_IsClosed = true;
		}
	}
	return 0;
}

int Lua_glColor4f(lua_State* L)
{
	glColor4f((float)lua_tonumber(L, -4) / 255.f, (float)lua_tonumber(L, -3) / 255.f, (float)lua_tonumber(L, -2) / 255.f, (float)lua_tonumber(L, -1) / 255.f);
	return 0;
}

int Lua_glColor3f(lua_State* L)
{

	glColor3f((float)lua_tonumber(L, -3) / 255.f, (float)lua_tonumber(L, -2) / 255.f, (float)lua_tonumber(L, -1) / 255.f);
	return 0;
}

int SetWindowPointer(lua_State* L)
{
	WindowPointer = lua_tointeger(L, -1);
	SDL_GL_MakeCurrent(WindowTable[WindowPointer], ContextTable[WindowPointer]);
	return 0;
}

int Lua_GlDraw2f(lua_State* L)
{
	glVertex2f((float)lua_tonumber(L, -1), (float)lua_tonumber(L, -2));
	return 0;
}
int Lua_glVertex2f(lua_State* L)
{
	glVertex2f((float)lua_tonumber(L, -2), (float)lua_tonumber(L, -1));
	glVertex2f((float)lua_tonumber(L, -4), (float)lua_tonumber(L, -3));
	return 0;
}
int Lua_MonitorEnum(lua_State* L)
{
	MonitorRects monitors;
	//monitors.rcMonitors.	lua_pushinteger(L, width);

	lua_newtable(L);

	for (int i = 0; i < monitors.rcMonitors.size(); i++)
	{


		lua_createtable(L, 4, 0);

		lua_pushlstring(L, "left", 4);
		lua_pushinteger(L, monitors.rcMonitors[i].left);
		lua_settable(L, -3);

		lua_pushlstring(L, "right", 5);
		lua_pushinteger(L, monitors.rcMonitors[i].right);
		lua_settable(L, -3);

		lua_pushlstring(L, "top", 3);
		lua_pushinteger(L, monitors.rcMonitors[i].top);
		lua_settable(L, -3);

		lua_pushlstring(L, "bottom", 6);
		lua_pushinteger(L, monitors.rcMonitors[i].bottom);
		lua_settable(L, -3);




		lua_rawseti(L, -2, i);
	}
	//monitors.rcMonitors[1].
	//std::cout << "You have " <<  << " monitors connected.";
	return 1;
}

int Lua_displayIsClosed(lua_State* L)
{
	lua_pushinteger(L, m_IsClosed);
	return 1;
}
int Lua_BitOR(lua_State* L)
{
	lua_pushinteger(L, lua_tointeger(L, -1) | lua_tointeger(L, -2));
	return 1;
}
int Lua_CheckMask(lua_State* L)
{
	lua_pushboolean(L, 0 != (lua_tointeger(L, -1) & lua_tointeger(L, -2)));
	return 1;
}
int GetMousePosition(lua_State* L)
{
	if (GetCursorPos(&p))
	{
		lua_pushinteger(L, p.x);
		lua_pushinteger(L, p.y);
		return 2;
	}
	return 0;
}

int Lua_ToggleVSync(lua_State* L)
{
	int Fail = SDL_GL_SetSwapInterval((int)lua_tonumber(L, -1));
	lua_pushinteger(L, Fail);
	return 1;
}
int SetAlpha(lua_State* L)
{
	SDL_SetWindowOpacity(WindowTable[WindowPointer], lua_tonumber(L, -1));
	return 0;
}
int MakeWindowSeethrough(lua_State* L)
{
	COLORREF idk = 0x00000000;
	MakeWindowTransparent(WindowTable[WindowPointer], idk);
	return 0;
}

int Lua_Sleep(lua_State* L)
{
	Sleep(lua_tonumber(L, -1));
	return 0;
}

int Lua_LoadTexture(lua_State* L)
{
	int width, height, channelCount;
	unsigned char* data = stbi_load(lua_tostring(L, -1), &width, &height, &channelCount, 0);

	if (data == NULL) {
		return 0;
	}



	lua_pushinteger(L, width);
	lua_pushinteger(L, height);
	lua_pushinteger(L, channelCount);
	lua_createtable(L, width * height, 0);
	unsigned bytePerPixel = channelCount;
	for (int i = 0; i < width * height; i++)
	{
		unsigned char* pixelOffset = data + (i)*bytePerPixel;

		lua_createtable(L, 4, 0);


		lua_pushlstring(L, "r", 1);
		lua_pushinteger(L, pixelOffset[0]);
		lua_settable(L, -3);

		lua_pushlstring(L, "g", 1);
		lua_pushinteger(L, pixelOffset[1]);
		lua_settable(L, -3);

		lua_pushlstring(L, "b", 1);
		lua_pushinteger(L, pixelOffset[2]);
		lua_settable(L, -3);

		lua_pushlstring(L, "a", 1);
		lua_pushinteger(L, channelCount >= 4 ? pixelOffset[3] : 0xff);
		lua_settable(L, -3);


		lua_rawseti(L, -2, i);


		//lua_createtable(L, 4, 0); 

		//lua_pushinteger(L, pixelOffset[0]);
		//lua_rawseti(L, -2, 1);

		//lua_pushinteger(L, pixelOffset[1]);
		//lua_rawseti(L, -2, 2);  

		//lua_pushinteger(L, pixelOffset[2]);
		//lua_rawseti(L, -2, 3);

		//lua_pushinteger(L, channelCount >= 4 ? pixelOffset[3] : 0xff);
		//lua_rawseti(L, -2, 4);

		//lua_rawseti(L, -2, i); 
	}
	free(data);
	return 4;
}


int Lua_LoadTexture_numtable(lua_State* L)
{
	int width, height, channelCount;
	unsigned char* data = stbi_load(lua_tostring(L, -1), &width, &height, &channelCount, 0);

	if (data == NULL) {
		return 0;
	}



	lua_pushinteger(L, width);
	lua_pushinteger(L, height);
	lua_pushinteger(L, channelCount);
	lua_createtable(L, width * height, 0);
	unsigned bytePerPixel = channelCount;
	for (int i = 0; i < width * height; i++)
	{
		unsigned char* pixelOffset = data + (i)*bytePerPixel;

		//lua_createtable(L, 4, 0);


		//lua_pushlstring(L, "r", 1);
		//lua_pushinteger(L, pixelOffset[0]);
		//lua_settable(L, -3);

		//lua_pushlstring(L, "g", 1);
		//lua_pushinteger(L, pixelOffset[1]);
		//lua_settable(L, -3);

		//lua_pushlstring(L, "b", 1);
		//lua_pushinteger(L, pixelOffset[2]);
		//lua_settable(L, -3);

		//lua_pushlstring(L, "a", 1);
		//lua_pushinteger(L, channelCount >= 4 ? pixelOffset[3] : 0xff);
		//lua_settable(L, -3);


		//lua_rawseti(L, -2, i);


		lua_createtable(L, 4, 0);

		lua_pushinteger(L, pixelOffset[0]);
		lua_rawseti(L, -2, 1);

		lua_pushinteger(L, pixelOffset[1]);
		lua_rawseti(L, -2, 2);

		lua_pushinteger(L, pixelOffset[2]);
		lua_rawseti(L, -2, 3);

		lua_pushinteger(L, channelCount >= 4 ? pixelOffset[3] : 0xff);
		lua_rawseti(L, -2, 4);

		lua_rawseti(L, -2, i);
	}
	free(data);
	return 4;
}

int Lua_FFB_Init(lua_State* L)
{
	HWNDProgram = GetForegroundWindow();
	printf("Initializing %p\n", &HWNDProgram);


	HRESULT hr;
	if (FAILED(hr = InitDirectInput()))
	{
		printf("[C++] InitDirectInput failed Errorcode: %d, Error message: %s\n", hr, formatError(hr).c_str());
	}
	return 0;
}

int Lua_FFB_Set(lua_State* L)
{

	g_nXForce = lua_tointeger(L, -1);
	HRESULT hr;
	if (FAILED(hr = SetDeviceForcesXY()))
	{
		//lua["print"]("[C++] SetDeviceForcesXY failed Errorcode : %d, Error message : %s\n", hr, formatError(hr).c_str());
		printf("[C++] SetDeviceForcesXY failed Errorcode: %d, Error message: %s\n", hr, formatError(hr).c_str());
	}
	return 0;
}



int Lua_XInput_Vibration(lua_State* L)
{
	XINPUT_VIBRATION brr = {};
	float LeftForce = lua_tonumber(L, -2) * 65535.f;
	float RightForce = lua_tonumber(L, -1) * 65535.f;
	if (LeftForce >= 65535.f) { LeftForce = 65535.f; }
	if (RightForce >= 65535.f) { RightForce = 65535.f; }


	brr.wLeftMotorSpeed = (int)LeftForce;
	brr.wRightMotorSpeed = (int)RightForce;
	
	lua_pushboolean(L, XInputSetState(lua_tointeger(L, -3), &brr) == ERROR_SUCCESS);
	return 1;
}
int Lua_GetVirtualDesktopSize(lua_State* L)
{
	int x_ = GetSystemMetrics(SM_XVIRTUALSCREEN);  //left (e.g. -1024)
	int y_ = GetSystemMetrics(SM_YVIRTUALSCREEN);  //top (e.g. -34)
	int cx_ = GetSystemMetrics(SM_CXVIRTUALSCREEN); //entire width (e.g. 2704)
	int cy_ = GetSystemMetrics(SM_CYVIRTUALSCREEN); //entire height (e.g. 1050)
	lua_pushinteger(L, x_);
	lua_pushinteger(L, y_);
	lua_pushinteger(L, cx_);
	lua_pushinteger(L, cy_);
	return 4;
}

int Lua_XInput_GetState(lua_State* L)
{
	XINPUT_STATE CTRLState = {};
	//CTRLState.Gamepad.
	
	if (XInputGetState(lua_tointeger(L,-1), &CTRLState) == ERROR_SUCCESS)
	{
		lua_pushinteger(L, CTRLState.Gamepad.bLeftTrigger);
		lua_pushinteger(L, CTRLState.Gamepad.bRightTrigger);
		lua_pushinteger(L, CTRLState.Gamepad.wButtons);
		return 3;
	}
	
	return 0;
}
int Lua_FFB_Free(lua_State* L)
{
	FreeDirectInput();
	return 0;
}
int Lua_FFB_Acquire(lua_State* L)
{
	HRESULT hr;
	if (FAILED(hr = g_pDevice->Acquire()))
	{
		printf("[C++] g_pDevice->Acquire failed Errorcode: %d, Error message: %s\n", hr, formatError(hr).c_str());
	}
	return 0;
}


int Lua_LoadTexture_NOSEND(lua_State* L)
{
	int width, height, channelCount;
	unsigned char* data = stbi_load(lua_tostring(L, -1), &width, &height, &channelCount, 0);

	if (data == NULL) {
		return 0;
	}

	free(data);
	return 0;
}


int Lua___readTableParams(lua_State* L)
{
	if (!lua_istable(L, -1))
		return 0;


	lua_rawgeti(L, 1, 1);
	lua_rawgeti(L, 1, 2); 
	lua_rawgeti(L, 1, 3); 
	printf("%f %f %f\n", lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
	lua_pop(L, 1);
	lua_pop(L, 1);
	lua_pop(L, 1);
	return 0;
}

BOOL CALLBACK enumProc(HWND hwnd, LPARAM) {
    TCHAR buf[1024]{};

    GetClassName(hwnd, buf, 100);
    //if (!lstrcmp(buf, L"Discord"))
    {
        GetWindowText(hwnd, buf, 100);
		std::wstring Title(buf);
		Title.c_str();
        //DWORD pid = 0;
		//Title.find
		//std::wstring::size_type found = Title.find(L"Discord", 99);
		//if (found != std::string::npos)
		{
			std::wcout << Title << hwnd << std::endl;
		}
        //GetWindowThreadProcessId(hwnd, &pid);
        //wcout << buf << " " << pid << endl;
		
    }
    return true;
}


//static BOOL CALLBACK enumWindowCallback(HWND hWnd, LPARAM lparam) {
//	int length = GetWindowTextLength(hWnd);
//	char* buffer = new char[length + 1];
//	//LPWSTR buffer = {};
//	GetWindowTextW(hWnd, buffer, length + 1);
//	
//	//std::string windowTitle(buffer);
//	delete[] buffer;
//	printf("%S", buffer);
//
//	//// List visible windows with a non-empty title
//	//if (IsWindowVisible(hWnd) && length != 0) {
//	//	std::cout << hWnd << ":  " << windowTitle << std::endl;
//	//}
//	//return TRUE;
//	return true;
//}

int Lua___EnumAutism(lua_State* L)
{
	EnumWindows(enumProc, NULL);
	return 0;
}
void flipVertically(int width, int height, char* data)
{
	char rgb[3];

	for (int y = 0; y < height / 2; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int top = (x + y * width) * 3;
			int bottom = (x + (height - y - 1) * width) * 3;

			memcpy(rgb, data + top, sizeof(rgb));
			memcpy(data + top, data + bottom, sizeof(rgb));
			memcpy(data + bottom, rgb, sizeof(rgb));
		}
	}
}

int Lua___saveDebugScreenshot(lua_State* L)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int x = viewport[0];
	int y = viewport[1];
	int width = viewport[2];
	int height = viewport[3];

	char* data = (char*)malloc((size_t)(width * height * 3)); // 3 components (R, G, B)

	if (!data)
		return 0;

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_BACK);
	//glReadBuffer(GL_BACK);
	//glReadPixels(…);

	glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

	flipVertically(width, height, data);

	int saved = stbi_write_png("debug_output_screenshot.png", width, height, 3, data, 0);

	free(data);
	lua_pushinteger(L, saved);
	return 1;
}

void convertBGRtoRGB(unsigned char* image, int width, int height)
{
	for (int i = 0; i < width * height * 4; i += 4) {
		unsigned char temp = image[i];
		image[i] = image[i + 2];
		image[i + 2] = temp;
	}
}
int Lua___saveDesktopScreenshot(lua_State* L)
{
	//int ScreenX = 0;
	//int ScreenY = 0;
	//BYTE* ScreenData = 0;
	
	std::string idk = lua_tostring(L, -1);
	convertBGRtoRGB(ScreenData, ScreenX, ScreenY);
	int saved = stbi_write_png(idk.c_str(), ScreenX, ScreenY, 4, ScreenData, 0);
	convertBGRtoRGB(ScreenData, ScreenX, ScreenY);
	//free(data);
	lua_pushinteger(L, saved);
	return 0;
}



extern "C" int __declspec(dllexport) luaopen_glwrapper_core(lua_State * L)
{

	lua_register(L, "EnableVSyncUnsafe", Lua_ToggleVSync);
	lua_register(L, "__readTableParams", Lua___readTableParams);
	lua_register(L, "CreateDisplay", CreateDisplay);
	lua_register(L, "SaveCurrentScreencap", Lua___saveDesktopScreenshot);
	lua_register(L, "DestroyDisplay", destroyDisplay);
	lua_register(L, "DisplayIsClosed", Lua_displayIsClosed);
	lua_register(L, "DisplayClear", displayClear);
	lua_register(L, "DisplayClearNoAlpha", displayClearNoAlpha);
	lua_register(L, "DisplayUpdate", Lua_displayUpdate);
	lua_register(L, "GlBeginLines", Lua_glBegLines);
	lua_register(L, "GlBeginPoints", Lua_glBegPoints);
	lua_register(L, "GlEnd", Lua_glEnd);
	lua_register(L, "EnumWindows", Lua___EnumAutism);
	lua_register(L, "SetColor", Lua_glColor3f);
	lua_register(L, "SetColorAlpha", Lua_glColor4f);
	lua_register(L, "DrawLine", Lua_glVertex2f);
	lua_register(L, "BitOR", Lua_BitOR);
	lua_register(L, "CheckMask", Lua_CheckMask);
	lua_register(L, "MoveWindow", MoveWindow);
	lua_register(L, "SaveDebugScreenshot", Lua___saveDebugScreenshot);
	lua_register(L, "GetMousePosition", GetMousePosition);
	lua_register(L, "ReadPixelColor", ReadPixelColor);
	lua_register(L, "ReadPixelColorUnsafeAsFuck", ReadPixelColorUnsafeAsFuck);
	lua_register(L, "CaptureScreen", ScreenCap);
	lua_register(L, "ScreenCapFull", ScreenCap_WholeScreen);
	lua_register(L, "LoadTexture", Lua_LoadTexture);
	lua_register(L, "LoadTextureNOSEND", Lua_LoadTexture_NOSEND);
	lua_register(L, "SetAlpha", SetAlpha);
	lua_register(L, "SetWindowPointer", SetWindowPointer);
	lua_register(L, "GlDraw2f", Lua_GlDraw2f);
	lua_register(L, "MakeWindowSeethrough", MakeWindowSeethrough);
	lua_register(L, "IsKeyDown", Lua_IsKeyDown);
	lua_register(L, "MonitorEnum", Lua_MonitorEnum);
	lua_register(L, "keybd_event", Lua_keybd_event);
	lua_register(L, "SendInput", Lua_SendClick);
	lua_register(L, "Sleep", Lua_Sleep);
	lua_register(L, "FFB_Init", Lua_FFB_Init);
	lua_register(L, "FFB_Acquire", Lua_FFB_Acquire);
	lua_register(L, "FFB_Set", Lua_FFB_Set);
	lua_register(L, "FFB_Free", Lua_FFB_Free);
	lua_register(L, "XInput_Vibration", Lua_XInput_Vibration);
	lua_register(L, "XInput_GetState", Lua_XInput_GetState);
	return 1;
}
