#define WINDOWS_IGNORE_PACKING_MISMATCH // XD
#include <GL/glew.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL.h>

#include <iostream>
#include <Windows.h>
//#include <string>



extern "C"
{
#include "lua51/include/lua.h"
#include "lua51/include/lauxlib.h"
#include "lua51/include/lualib.h"
}
#pragma comment(lib, "lua51/lua5.1.lib")


//extern "C"
//{
//#include "luajit210b3/include/lua.h"
//#include "luajit210b3/include/lauxlib.h"
//#include "luajit210b3/include/lualib.h"
//}
//#pragma comment(lib, "luajit210b3/lua51.lib")


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

POINT p;
SDL_Window* WindowTable[50];
SDL_GLContext ContextTable[50];
int WindowPointer = 0;
bool m_IsClosed = false;
int ScreenX = 0;
int ScreenY = 0;
BYTE* ScreenData = 0;

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


int ScreenCap(lua_State* L)
{
	HDC hScreen = GetDC(NULL);
	ScreenX = GetDeviceCaps(hScreen, HORZRES);
	ScreenY = GetDeviceCaps(hScreen, VERTRES);

	HDC hdcMem = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, ScreenX, ScreenY);
	HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);
	BitBlt(hdcMem, 0, 0, ScreenX, ScreenY, hScreen, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hOld);

	BITMAPINFOHEADER bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biWidth = ScreenX;
	bmi.biHeight = -ScreenY;
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;
	//printf("w=%d, h=%d\n", ScreenX , ScreenY);
	if (ScreenData)
		free(ScreenData);
	ScreenData = (BYTE*)malloc(4 * ScreenX * ScreenY);

	GetDIBits(hdcMem, hBitmap, 0, ScreenY, ScreenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

	ReleaseDC(GetDesktopWindow(), hScreen);
	DeleteDC(hdcMem);
	DeleteObject(hBitmap);
	return 0;
}

int ReadPixelColor(lua_State* L)
{
	int x = (int)lua_tonumber(L, -2);
	int y = (int)lua_tonumber(L, -1);
	//printf("x=%d, y=%d\n",x,y);
	
	if (x >= 0 && x <= ScreenX && y >= 0 && y <= ScreenY)
	{
		lua_pushinteger(L, ScreenData[4 * ((y * ScreenX) + x) + 2]);
		lua_pushinteger(L, ScreenData[4 * ((y * ScreenX) + x) + 1]);
		lua_pushinteger(L, ScreenData[4 * ((y * ScreenX) + x)]);
		return 3;
	}
	return 0;
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


extern "C" int __declspec(dllexport) luaopen_glwrapper_core(lua_State * L)
{
	lua_register(L, "EnableVSyncUnsafe", Lua_ToggleVSync);
	lua_register(L, "CreateDisplay", CreateDisplay);
	lua_register(L, "DestroyDisplay", destroyDisplay);
	lua_register(L, "DisplayIsClosed", Lua_displayIsClosed);
	lua_register(L, "DisplayClear", displayClear);
	lua_register(L, "DisplayClearNoAlpha", displayClearNoAlpha);
	lua_register(L, "DisplayUpdate", Lua_displayUpdate);
	lua_register(L, "GlBeginLines", Lua_glBegLines);
	lua_register(L, "GlBeginPoints", Lua_glBegPoints);
	lua_register(L, "GlEnd", Lua_glEnd);
	lua_register(L, "SetColor", Lua_glColor3f);
	lua_register(L, "SetColorAlpha", Lua_glColor4f);
	lua_register(L, "DrawLine", Lua_glVertex2f);
	lua_register(L, "BitOR", Lua_BitOR);
	lua_register(L, "MoveWindow", MoveWindow);
	lua_register(L, "GetMousePosition", GetMousePosition);
	lua_register(L, "ReadPixelColor", ReadPixelColor);
	lua_register(L, "CaptureScreen", ScreenCap);
	lua_register(L, "LoadTexture", Lua_LoadTexture);
	lua_register(L, "SetAlpha", SetAlpha);
	lua_register(L, "SetWindowPointer", SetWindowPointer);
	lua_register(L, "GlDraw2f", Lua_GlDraw2f);
	lua_register(L, "MakeWindowSeethrough", MakeWindowSeethrough);
	lua_register(L, "IsKeyDown", Lua_IsKeyDown);
	lua_register(L, "keybd_event", Lua_keybd_event);
	lua_register(L, "Sleep", Lua_Sleep);
	return 1;
}
