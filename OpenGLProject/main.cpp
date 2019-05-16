#include <GL/glew.h>
#include <SDL2/SDL.h>
//#include <iostream>
//#include <string>

extern "C"
{
#include "lua51/include/lua.h"
#include "lua51/include/lauxlib.h"
#include "lua51/include/lualib.h"
}

#ifdef _WIN32
#pragma comment(lib, "lua51/lua5.1.lib")
#endif

using namespace std;

SDL_Window* m_window;
SDL_GLContext m_glContext;
bool m_IsClosed = false;

int main(int argc, char *argv[])
{
	return 1;
}

int CreateDisplay(lua_State *L)
{
	int width = (int)lua_tonumber(L, -2);
	int height = (int)lua_tonumber(L, -1);
	//std::string title = lua_tostring(L, -1);

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

	m_window = SDL_CreateWindow("Lua OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	m_glContext = SDL_GL_CreateContext(m_window);

	GLenum status = glewInit();

	if (status != GLEW_OK)
	{
		//std::cerr << "Glew failed to initialize!" << std::endl;
	}
	m_IsClosed = false;
	return 1;
}

int destroyDisplay(lua_State *L)
{
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
	return 1;
}

int Lua_glBegLines(lua_State *L)
{
	glBegin(GL_LINES);
	return 1;
}
int Lua_glEnd(lua_State *L)
{
	glEnd();
	return 1;
}
int displayClear(lua_State *L)
{
	glClearColor(0.f,0.f,0.f,1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	return 1;
}

int Lua_displayUpdate(lua_State *L)
{
	SDL_GL_SwapWindow(m_window);
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT) {
			m_IsClosed = true;
		}
	}
	return 1;
}

int Lua_glColor3f(lua_State *L)
{
	glColor3f((float)lua_tonumber(L, -3), (float)lua_tonumber(L, -2), (float)lua_tonumber(L, -1));
	return 1;
}

int Lua_glVertex2f(lua_State *L)
{
	glVertex2f((float)lua_tonumber(L, -2), (float)lua_tonumber(L, -1));
	glVertex2f((float)lua_tonumber(L, -4), (float)lua_tonumber(L, -3));
	return 1;
}

int Lua_displayIsClosed(lua_State *L)
{
	return m_IsClosed;
}

int Lua_ToggleVSync(lua_State *L)
{
	SDL_GL_SetSwapInterval((int)lua_tonumber(L, -1));
	return 1;
}

extern "C" int __declspec(dllexport) luaopen_glwrapper_core(lua_State *L)
{
	lua_register(L, "EnableVSyncUnsafe", Lua_ToggleVSync);
	lua_register(L, "CreateDisplayUnsafe", CreateDisplay);
	lua_register(L, "DestroyDisplay", destroyDisplay);
	lua_register(L, "DisplayIsClosed", Lua_displayIsClosed);
	lua_register(L, "DisplayClear", displayClear);
	lua_register(L, "DisplayUpdate", Lua_displayUpdate);
	lua_register(L, "GlBeginLines", Lua_glBegLines);
	lua_register(L, "GlEnd", Lua_glEnd);
	lua_register(L, "GlSetColorUnsafe", Lua_glColor3f);
	lua_register(L, "GlDrawLineUnsafe", Lua_glVertex2f);
	return 1;
}
