#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "include/graphics/graphics.h"
#include "include/luaplayer.h"
#include "include/utils.h"
#define u32 uint32_t

static int lua_newT(lua_State *L) {
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
       if (argc != 0) return luaL_error(L, "wrong number of arguments.");
	#endif
	Timer* new_timer = (Timer*)malloc(sizeof(Timer));
	
	// Calculating current frame
	new_timer->tick = osGetTime();
	new_timer->magic = 0x4C544D52;
	new_timer->isPlaying = true;
	
	drawCommand("Timer.new: ","Timer created at offset 0x%lX.\n",(u32)new_timer);
	allocatedDatas = allocatedDatas + sizeof(Timer);
	size_t percent = (allocatedDatas * 100) / maxDatas;
	drawDebug("Debug: ","Allocated %lu bytes (Mem. usage: %u%%).\n",sizeof(Timer), percent);
	if (percent > 80) drawWarning("Warning: ", "High memory usage!");
	if (percent > 100){
		drawError("FATAL ERROR: ", "Out of memory!");
		return luaL_error(L, "internal error.");
	}
	
    lua_pushinteger(L,(u32)new_timer);
    return 1;
}

static int lua_time(lua_State *L) {
    int argc = lua_gettop(L);
    #ifndef SKIP_ERROR_HANDLING
       if (argc != 1) return luaL_error(L, "wrong number of arguments.");
	#endif
    Timer* src = (Timer*)luaL_checkinteger(L,1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	if (src->isPlaying){
		lua_pushinteger(L, (osGetTime() - src->tick));
	}else{
		lua_pushinteger(L, src->tick);
	}
    return 1;
}

static int lua_pause(lua_State *L)
{
    int argc = lua_gettop(L);
    #ifndef SKIP_ERROR_HANDLING
       if (argc != 1) return luaL_error(L, "wrong number of arguments.");
	#endif
	Timer* src = (Timer*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	if (src->isPlaying){
		src->isPlaying = false;
		src->tick = (osGetTime()-src->tick);
	}
	return 0;
}

static int lua_resume(lua_State *L)
{
    int argc = lua_gettop(L);
    #ifndef SKIP_ERROR_HANDLING
       if (argc != 1) return luaL_error(L, "wrong number of arguments.");
	#endif
	Timer* src = (Timer*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	if (!src->isPlaying){
		src->isPlaying = true;
		src->tick = (osGetTime()-src->tick);
	}
	return 0;
}

static int lua_reset(lua_State *L)
{
    int argc = lua_gettop(L);
    #ifndef SKIP_ERROR_HANDLING
       if (argc != 1) return luaL_error(L, "wrong number of arguments.");
	#endif
	Timer* src = (Timer*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	if (src->isPlaying) src->tick = osGetTime();
	else src->tick = 0;
	return 0;
}

static int lua_wisPlaying(lua_State *L){
	int argc = lua_gettop(L);
    #ifndef SKIP_ERROR_HANDLING
       if (argc != 1) return luaL_error(L, "wrong number of arguments.");
	#endif
	Timer* src = (Timer*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	lua_pushboolean(L, src->isPlaying);
	return 1;
}

static int lua_destroy(lua_State *L) {
    int argc = lua_gettop(L);
    #ifndef SKIP_ERROR_HANDLING
       if (argc != 1) return luaL_error(L, "wrong number of arguments.");
	#endif
    Timer* timer = (Timer*)luaL_checkinteger(L,1);
	#ifndef SKIP_ERROR_HANDLING
		if (timer->magic != 0x4C544D52) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	free(timer);
	allocatedDatas = allocatedDatas - sizeof(Timer);
	size_t percent = (allocatedDatas * 100) / maxDatas;
	drawDebug("Debug: ","Deallocated %lu bytes (Mem. usage: %u%%).\n",sizeof(Timer), percent);
    return 1;
}

//Register our Timer Functions
static const luaL_Reg Timer_functions[] = {
  {"new",							lua_newT},
  {"getTime",						lua_time},
  {"destroy",						lua_destroy},
  {"pause",							lua_pause},
  {"resume",						lua_resume},
  {"reset",							lua_reset},
  {"isPlaying",						lua_wisPlaying},
  {0, 0}
};

void luaTimer_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, Timer_functions, 0);
	lua_setglobal(L, "Timer");
}