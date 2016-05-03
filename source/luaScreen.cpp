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
#include "include/luaplayer.h"
#include "include/graphics/graphics.h"
#define u8 uint8_t
#define u32 uint32_t
#define stringify(str) #str
#define VariableRegister(lua, value) do { lua_pushinteger(lua, value); lua_setglobal (lua, stringify(value)); } while(0)
extern bool quit;

static int lua_flip(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	drawTopFrame();
	drawBottomFrame();
	SDL_GL_SwapBuffers();
	SDL_Event event;
	while( SDL_PollEvent( &event ) ) {
		if( event.type == SDL_QUIT ) {
			char string[20];
			strcpy(string,"lpp_exit_0456432");
			luaL_dostring(L, "collectgarbage()");
			return luaL_error(L, string); // NOTE: This is a fake error
		} 
	}
	return 0;
}

static int lua_print(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if ((argc != 5) && (argc != 6)) return luaL_error(L, "wrong number of arguments");
	#endif
	int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
	char* text = (char*)(luaL_checkstring(L, 3));
	u32 color = luaL_checkinteger(L,4);
	u8 alpha = (color >> 24) & 0xFF;
	int screen = luaL_checkinteger(L,5);
	int side=0;
	if (argc == 6) side = luaL_checkinteger(L,6);
	#ifndef SKIP_ERROR_HANDLING
		if ((x < 0) || (y < 0)) return luaL_error(L, "out of bounds");
		if ((screen == 0) && (x > 400)) return luaL_error(L, "out of framebuffer bounds");
		if ((screen == 1) && (x > 320)) return luaL_error(L, "out of framebuffer bounds");
		if ((screen <= 1) && (y > 227)) return luaL_error(L, "out of framebuffer bounds");
	#endif
	DrawScreenText(x,y,text,color,screen,side);
	return 0;
}

static int lua_refresh(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	glBindTexture( GL_TEXTURE_2D, TopFB );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, 3, 400, 240, 0, texture_format, GL_UNSIGNED_BYTE, TopLFB );
	glBindTexture( GL_TEXTURE_2D, BottomFB );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, 3, 320, 240, 0, texture_format, GL_UNSIGNED_BYTE, BottomLFB );
	return 0;
}

static int lua_vblank(lua_State *L){
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	return 0;
}

//Register our Console Functions
static const luaL_Reg Console_functions[] = {
  {0, 0}
};

//Register our Color Functions
static const luaL_Reg Color_functions[] = {
  {0, 0}
};

//Register our Screen Functions
static const luaL_Reg Screen_functions[] = {
  {"debugPrint",		lua_print},
  {"flip",				lua_flip},
  {"refresh",			lua_refresh},
  {"waitVblankStart",	lua_vblank},
  {0, 0}
};

//Register our Font Functions
static const luaL_Reg Font_functions[] = {
  {0, 0}
};

void luaScreen_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, Screen_functions, 0);
	lua_setglobal(L, "Screen");
	lua_newtable(L);
	luaL_setfuncs(L, Font_functions, 0);
	lua_setglobal(L, "Font");
	lua_newtable(L);
	luaL_setfuncs(L, Color_functions, 0);
	lua_setglobal(L, "Color");
	lua_newtable(L);
	luaL_setfuncs(L, Console_functions, 0);
	lua_setglobal(L, "Console");
	int TOP_SCREEN = 0;
	int BOTTOM_SCREEN = 1;
	int LEFT_EYE = 0;
	int RIGHT_EYE = 1;
	VariableRegister(L,TOP_SCREEN);
	VariableRegister(L,BOTTOM_SCREEN);
	VariableRegister(L,LEFT_EYE);
	VariableRegister(L,RIGHT_EYE);
}