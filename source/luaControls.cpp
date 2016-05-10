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
#----------------------------------------------------------------------------------------------------------------------*/
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include "include/luaplayer.h"
#include "include/graphics/graphics.h"
#define stringify(str) #str
#define VariableRegister(lua, value) do { lua_pushinteger(lua, value); lua_setglobal (lua, stringify(value)); } while(0)
#define u32 uint32_t

// PC Keys Mapping Config
int KEY_HOME = SDLK_SPACE;
int KEY_POWER = SDLK_RETURN;
int KEY_A = SDLK_l;
int KEY_B = SDLK_k;	
int KEY_X = SDLK_i;
int KEY_Y = SDLK_j;
int KEY_DUP = SDLK_w;
int KEY_DDOWN = SDLK_s;
int KEY_DLEFT = SDLK_a;
int KEY_DRIGHT = SDLK_d;
int KEY_R = SDLK_o;
int KEY_L = SDLK_q;
int KEY_ZR = SDLK_u;
int KEY_ZL = SDLK_e;
int KEY_START = SDLK_LCTRL;
int KEY_SELECT = SDLK_LALT;
int KEY_TOUCH = SDLK_g; // Fake button, SDL_BUTTON_LEFT is the original state used

// Controls handles
uint8_t* ctrl_handles[CTRL_HANDLES];
int current_handle = 0;

static int lua_readC(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments.");
	#endif
	SDL_PumpEvents();
	int tab_size;
	uint8_t* status_tab = SDL_GetKeyState(&tab_size);
	if (ctrl_handles[current_handle] != NULL) free(ctrl_handles[current_handle]);
	ctrl_handles[current_handle] = (uint8_t*)malloc(tab_size);
	memcpy(ctrl_handles[current_handle],status_tab,tab_size);
	
	// Replacing unused key (SDLK_g) with mouse state for touchscreen emulation
	uint8_t mouse_state = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT));
	ctrl_handles[current_handle][SDLK_g] = mouse_state;
	
	lua_pushinteger(L, (u32)ctrl_handles[current_handle]);
	current_handle++;
	if (current_handle >= CTRL_HANDLES) current_handle = 0;
	return 1;
}

static int lua_readtouch(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments.");
	#endif
	SDL_PumpEvents();
	int x;
	int y;
	if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT)){
		x = x - 40;
		y = y - 240;
		if (x < 0) x = 0;
		else if (x > 320) x = 320;
		if (y < 0) y = 0;
		else if (y > 240) y = 240;
	}else{
		x = 0;
		y = 0;
	}
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	return 2;
}

static int lua_check(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 2) return luaL_error(L, "wrong number of arguments.");
	#endif
	uint8_t* pad = (uint8_t*)luaL_checkinteger(L, 1);
	u32 button = luaL_checkinteger(L, 2);
	lua_pushboolean(L, pad[button]);
    return 1;
}

static int lua_shell(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
       if (argc != 0) return luaL_error(L, "wrong number of arguments.");
	#endif
	drawCommand("Controls.shellStatus: ","Returning true as result.\n");
	lua_pushboolean(L, true);
	return 1;
}

//Register our Controls Functions
static const luaL_Reg Controls_functions[] = {
  {"read",								lua_readC},
  {"readTouch",							lua_readtouch},
  {"check",								lua_check},
  {"shellStatus", 						lua_shell},
  {0, 0}
};

void luaControls_init(lua_State *L) {

	for (int i=0; i < CTRL_HANDLES; i++){
		ctrl_handles[i] = NULL;
	}
	
	lua_newtable(L);
	luaL_setfuncs(L, Controls_functions, 0);
	lua_setglobal(L, "Controls");
	
	VariableRegister(L,KEY_HOME);
	VariableRegister(L,KEY_POWER);
	VariableRegister(L,KEY_A);
	VariableRegister(L,KEY_B);
	VariableRegister(L,KEY_SELECT);
	VariableRegister(L,KEY_START);
	VariableRegister(L,KEY_DRIGHT);
	VariableRegister(L,KEY_DLEFT);
	VariableRegister(L,KEY_DUP);
	VariableRegister(L,KEY_DDOWN);
	VariableRegister(L,KEY_R);
	VariableRegister(L,KEY_L);
	VariableRegister(L,KEY_X);
	VariableRegister(L,KEY_Y);
	VariableRegister(L,KEY_ZL);
	VariableRegister(L,KEY_ZR);
	VariableRegister(L,KEY_TOUCH);
}