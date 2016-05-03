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
	if (screen <= 1) DrawScreenText(x,y,text,color,screen,side);
	else{
		drawWarning("Warning: ","Skipped target image checks.\n");
		DrawImageText(x,y,text,color,(SDL_Surface*)screen);
	}
	return 0;
}

static int lua_freeimg(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
    if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	SDL_Surface* img = (SDL_Surface*)(luaL_checkinteger(L, 1));
	SDL_FreeSurface(img);
	return 0;
}

static int lua_loadimg(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
    if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	char* text = (char*)(luaL_checkstring(L, 1));
	SDL_Surface* img = IMG_Load(text);
	drawCommand("Screen.loadImage: ","Image created at offset 0x%lX.\n",img);
	if (img == NULL) return luaL_error(L, "error while loading image.");
    lua_pushinteger(L, (u32)(img));
	return 1;
}

static int lua_drawimg(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if ((argc != 4) && (argc != 5)) return luaL_error(L, "wrong number of arguments");
	#endif
	int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
	SDL_Surface* file = (SDL_Surface*)luaL_checkinteger(L, 3);
	drawCommand("Screen.drawImage: ","Drawing image at offset 0x%lX.\n",file);
	int screen= luaL_checkinteger(L, 4);
	int side = 0;
	if (argc == 5) side = luaL_checkinteger(L,5);
	#ifndef SKIP_ERROR_HANDLING
		if ((x < 0) || (y < 0) || (y > 240)) return luaL_error(L, "out of framebuffer bounds");
		if ((screen == 0) && (x > 400)) return luaL_error(L, "out of framebuffer bounds");
		if ((screen == 1) && (x > 320)) return luaL_error(L, "out of framebuffer bounds");
	#endif
	SDL_LockSurface(file);
	if (screen == 0){
		bool partial_x = false;
		bool partial_y = false;
		if (file->w > 400) partial_x = true;
		if (file->h > 240) partial_y = true;
		if (partial_x || partial_y){
			drawWarning("Warning: ","Image is too big, auto-crop will be performed.\n");
			int width = file->w;
			int height = file->h;
			if (partial_x) width = 400-x;
			if (partial_y) height = 240-y;
			//PrintPartialScreenBitmap(x,y,0,0,width,height,file,screen,side);
		}else PrintScreenImage(x,y,file,screen,side);
	}else{
		bool partial_x = false;
		bool partial_y = false;
		if (file->w > 320) partial_x = true;
		if (file->h > 240) partial_y = true;
		if (partial_x || partial_y){
			drawWarning("Warning: ","Image is too big, auto-crop will be performed.\n");
			int width = file->w;
			int height = file->h;
			if (partial_x) width = 320-x;
			if (partial_y) height = 240-y;
			//PrintPartialScreenBitmap(x,y,0,0,width,height,file,screen,side);
		}else PrintScreenImage(x,y,file,screen,side);
	}
	SDL_UnlockSurface(file);
	return 0;
}

static int lua_pixel(lua_State *L){
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if ((argc != 4) && (argc != 5)) return luaL_error(L, "wrong number of arguments");
	#endif
	int x = luaL_checkinteger(L,1);
	int y = luaL_checkinteger(L,2);
	u32 color = luaL_checkinteger(L,3);
	u8 alpha = (color >> 24) & 0xFF;
	int screen = luaL_checkinteger(L,4);
	int side=0;
	if (argc == 5) side = luaL_checkinteger(L,5);
	#ifndef SKIP_ERROR_HANDLING
		if ((x < 0) || (y < 0)) return luaL_error(L, "out of bounds");
		if ((screen == 0) && (x > 400)) return luaL_error(L, "out of framebuffer bounds");
		if ((screen == 1) && (x > 320)) return luaL_error(L, "out of framebuffer bounds");
		if ((screen <= 1) && (y > 240)) return luaL_error(L, "out of framebuffer bounds");
	#endif
	u8* buffer;
		if (screen == 0){
			if (side == 0) buffer = TopLFB;
			else return 0;
		}else if (screen == 1) buffer = BottomLFB;
	DrawPixel(buffer,x,y,color);
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
  {"loadImage",			lua_loadimg},
  {"drawImage",			lua_drawimg},
  {"freeImage",			lua_freeimg},
  {"debugPrint",		lua_print},
  {"drawPixel",			lua_pixel},
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