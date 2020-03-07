#include <GL/glew.h>
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

#ifdef _WIN32
#pragma comment(lib, "lua51/lua5.1.lib")
//#define WIN32_LEAN_AND_MEAN
//#pragma comment(linker,"/ENTRY:luaopen_glwrapper_core")
#endif

//using namespace std;
POINT p;
//SDL_Window* m_window;
//SDL_GLContext m_glContext;


SDL_Window* WindowTable[50];
SDL_GLContext ContextTable[50];
int WindowPointer = 0;
bool m_IsClosed = false;

int main(int argc, char *argv[])
{
	return 1;
}

int CreateDisplay(lua_State *L)
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

int MoveWindow(lua_State *L)
{
	int x = (int)lua_tonumber(L, -2);
	int y = (int)lua_tonumber(L, -1);

	SDL_SetWindowPosition(WindowTable[WindowPointer],x,y);
	return 0;
}

int destroyDisplay(lua_State *L)
{
	SDL_GL_DeleteContext(ContextTable[WindowPointer]);
	SDL_DestroyWindow(WindowTable[WindowPointer]);
	SDL_Quit();
	return 0;
}

int Lua_glBegLines(lua_State *L)
{
	glBegin(GL_LINES);
	return 0;
}
int Lua_glEnd(lua_State *L)
{
	glEnd();
	return 0;
}
int displayClear(lua_State *L)
{
	glClearColor(0.f,0.f,0.f,1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	return 0;
}
int displayClearNoAlpha(lua_State *L)
{
	glClearColor(0.f,0.f,0.f,0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_STENCIL_BUFFER_BIT);
	glClear(GL_ACCUM_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	return 0;
}

int Lua_displayUpdate(lua_State *L)
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

int Lua_glColor4f(lua_State *L)
{
	glColor4f((float)lua_tonumber(L, -4), (float)lua_tonumber(L, -3), (float)lua_tonumber(L, -2), (float)lua_tonumber(L, -1));
	return 0;
}

int Lua_glColor3f(lua_State *L)
{

	glColor3f((float)lua_tonumber(L, -3), (float)lua_tonumber(L, -2), (float)lua_tonumber(L, -1));
	return 0;
}

int SetWindowPointer(lua_State *L)
{
	WindowPointer = lua_tointeger(L, -1);
	SDL_GL_MakeCurrent(WindowTable[WindowPointer], ContextTable[WindowPointer]);
	return 0;
}

int Lua_glVertex2f(lua_State *L)
{
	glVertex2f((float)lua_tonumber(L, -2), (float)lua_tonumber(L, -1));
	glVertex2f((float)lua_tonumber(L, -4), (float)lua_tonumber(L, -3));
	return 0;
}

int Lua_displayIsClosed(lua_State *L)
{
	lua_pushinteger(L, m_IsClosed);
	return 1;
}
int Lua_BitOR(lua_State *L)
{
	lua_pushinteger(L, lua_tointeger(L, -1) | lua_tointeger(L, -2));
	return 1;
}
int GetMousePosition(lua_State *L)
{
	if (GetCursorPos(&p))
	{
		lua_pushinteger(L, p.x);
		lua_pushinteger(L, p.y);
		return 2;
	}
	return 0;
}

int Lua_ToggleVSync(lua_State *L)
{
	int Fail = SDL_GL_SetSwapInterval((int)lua_tonumber(L, -1));
	lua_pushinteger(L,Fail);
	return 1;
}
int SetAlpha(lua_State *L)
{
	SDL_SetWindowOpacity(WindowTable[WindowPointer], lua_tonumber(L, -1));
	return 0;
}

extern "C" int __declspec(dllexport) luaopen_glwrapper_core(lua_State *L)
{
	lua_register(L, "EnableVSyncUnsafe", Lua_ToggleVSync);
	lua_register(L, "CreateDisplayUnsafe", CreateDisplay);
	lua_register(L, "DestroyDisplay", destroyDisplay);
	lua_register(L, "DisplayIsClosed", Lua_displayIsClosed);
	lua_register(L, "DisplayClear", displayClear);
	lua_register(L, "DisplayClearNoAlpha", displayClearNoAlpha);
	lua_register(L, "DisplayUpdate", Lua_displayUpdate);
	lua_register(L, "GlBeginLines", Lua_glBegLines);
	lua_register(L, "GlEnd", Lua_glEnd);
	lua_register(L, "GlSetColorUnsafe", Lua_glColor3f);
	lua_register(L, "GlSetColorUnsafeAlpha", Lua_glColor4f);
	lua_register(L, "GlDrawLineUnsafe", Lua_glVertex2f);
	lua_register(L, "BitOR", Lua_BitOR);
	lua_register(L, "MoveWindow", MoveWindow);
	lua_register(L, "GetMousePosition", GetMousePosition);
	lua_register(L, "SetAlpha", SetAlpha);
	lua_register(L, "SetWindowPointer", SetWindowPointer);
	return 1;
}
