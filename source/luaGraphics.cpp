/*----------------------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#------  This File is Part Of : ----------------------------------------------------------------------------------------#
#------- _  -------------------  ______   _   --------------------------------------------------------------------------#
#------ | | ------------------- (_____ \ | |  --------------------------------------------------------------------------#
#------ | | ---  _   _   ____    _____) )| |  ____  _   _   ____   ____   ----------------------------------------------#
#------ | | --- | | | | / _  |  |  ____/ | | / _  || | | | / _  ) / ___)  ----------------------------------------------#
#------ | |_____| |_| |( ( | |  | |      | |( ( | || |_| |( (/ / | |  --------------------------------------------------#
#------ |_______)\____| \_||_|  |_|      |_| \_||_| \__  | \____)|_|  --------------------------------------------------#
#------------------------------------------------- (____/  -------------------------------------------------------------#
#------------------------   ______   _   -------------------------------------------------------------------------------#
#------------------------  (_____ \ | |  -------------------------------------------------------------------------------#
#------------------------   _____) )| | _   _   ___   ------------------------------------------------------------------#
#------------------------  |  ____/ | || | | | /___)  ------------------------------------------------------------------#
#------------------------  | |      | || |_| ||___ |  ------------------------------------------------------------------#
#------------------------  |_|      |_| \____|(___/   ------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#- Licensed under the GPL License --------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#- Copyright (c) Nanni <lpp.nanni@gmail.com> ---------------------------------------------------------------------------#
#- Copyright (c) Rinnegatamante <rinnegatamante@gmail.com> -------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#----------------------------------------------------------------------------------------------------------------------*/

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "include/luaplayer.h"
#include "include/graphics/graphics.h"
#include "include/utils.h"
#include "include/audio.h"
#define stringify(str) #str
#define VariableRegister(lua, value) do { lua_pushinteger(lua, value); lua_setglobal (lua, stringify(value)); } while(0)
#define u16 uint16_t
#define u8 uint8_t
#define u32 uint32_t
#define u64 uint64_t

bool gpu_init = false;
int cur_screen = 0;

static int lua_end(lua_State *L) {
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	if (!gpu_init){
		drawError("Error: ", "GPU is not intialized.");
		return 0;
	}
    cur_screen = 0;
    return 0;
}

static int lua_start(lua_State *L) {
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 2) return luaL_error(L, "wrong number of arguments");
	#endif
	int screen = luaL_checkinteger(L,1);
	int side=0;
	if (argc == 2) side = luaL_checkinteger(L,2);
	if (!gpu_init){
		drawError("Error: ", "GPU is not intialized.");
		return 0;
	}
    cur_screen = screen;
	clearScreen(cur_screen);
    return 0;
}

static int lua_init(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	gpu_init = true;
	return 0;
}

static int lua_term(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	gpu_init = false;
	return 0;
}

//Register our Graphics Functions
static const luaL_Reg Graphics_functions[] = {
  {"init",					lua_init},
  {"term",					lua_term},
  {"initBlend",				lua_start},
  {"termBlend",				lua_end},
  {0, 0}
};

void luaGraphics_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, Graphics_functions, 0);
	lua_setglobal(L, "Graphics");
	u8 BORDER = 1;
	u8 CENTER = 3;
	VariableRegister(L,BORDER);
	VariableRegister(L,CENTER);
}