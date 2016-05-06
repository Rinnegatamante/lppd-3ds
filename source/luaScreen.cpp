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
#include "include/utils.h"
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
	unsigned long long tmp = getTotalSystemMemory();
	SDL_FreeSurface(img);
	unsigned long long tmp2 = getTotalSystemMemory();
	size_t allocatedDim = tmp-tmp2;
	allocatedDatas = allocatedDatas - allocatedDim;
	size_t percent = (allocatedDatas * 100) / maxDatas;
	drawDebug("Debug: ","Deallocated %lu bytes (Mem. usage: %u%%).\n",allocatedDim, percent);
	return 0;
}

static int lua_loadimg(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
    if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	char* text = (char*)(luaL_checkstring(L, 1));
	unsigned long long tmp = getTotalSystemMemory();
	SDL_Surface* img = IMG_Load(text);
	unsigned long long tmp2 = getTotalSystemMemory();
	drawCommand("Screen.loadImage: ","Image created at offset 0x%lX.\n",img);
	size_t allocatedDim = tmp2-tmp;
	allocatedDatas = allocatedDatas + allocatedDim;
	size_t percent = (allocatedDatas * 100) / maxDatas;
	drawDebug("Debug: ","Allocated %lu bytes (Mem. usage: %u%%).\n",allocatedDim, percent);
	if (percent > 80) drawWarning("Warning: ", "High memory usage!");
	if (percent > 100){
		drawError("FATAL ERROR: ", "Out of memory!");
		return luaL_error(L, "internal error.");
	}
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
			PrintPartialScreenImage(x,y,0,0,width,height,file,screen,side);
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
			PrintPartialScreenImage(x,y,0,0,width,height,file,screen,side);
		}else PrintScreenImage(x,y,file,screen,side);
	}
	return 0;
}

static int lua_partial(lua_State *L){
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if ((argc != 8) && (argc != 9)) return luaL_error(L, "wrong number of arguments");
	#endif
	int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
	int st_x = luaL_checkinteger(L, 3);
    int st_y = luaL_checkinteger(L, 4);
	int width = luaL_checkinteger(L, 5);
    int height = luaL_checkinteger(L, 6);
	SDL_Surface* file = (SDL_Surface*)luaL_checkinteger(L, 7);
	int screen= luaL_checkinteger(L, 8);
	int side = 0;
	if (argc == 9) side = luaL_checkinteger(L,9);
	#ifndef SKIP_ERROR_HANDLING
		if ((x < 0) || (y < 0)) return luaL_error(L, "out of bounds");
		if ((st_x < 0) || (st_y < 0)) return luaL_error(L, "out of image bounds");
		if (((st_x + width) > file->w) || (((st_y + height) > file->h))) return luaL_error(L, "out of image bounds");
		if ((screen == 0) && ((x+width) > 400)) return luaL_error(L, "out of framebuffer bounds");
		if ((screen == 1) && ((x+width) > 320)) return luaL_error(L, "out of framebuffer bounds");
		if ((screen <= 1) && ((y+height) > 240)) return luaL_error(L, "out of framebuffer bounds");
	#endif
	drawCommand("Screen.drawPartialImage: ","Drawing image at offset 0x%lX.\n",file);
	if (screen > 1){
		drawWarning("Warning: ","Skipped target image checks.\n");
		PrintPartialImageImage(x,y,st_x,st_y,width,height,file,(SDL_Surface*)screen);
	}else PrintPartialScreenImage(x,y,st_x,st_y,width,height,file,screen,side);
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
	if (screen > 1){
		SDL_Surface* target = (SDL_Surface*)screen;
		if (target->format->BitsPerPixel == 24) DrawImagePixel((u8*)target->pixels, x, y, color, target->w);
		else Draw32bppImagePixel((u8*)target->pixels, x, y, color, target->w);
	}else{
		if (alpha == 0xFF) DrawPixel(buffer,x,y,color);
		else DrawAlphaPixel(buffer,x,y,color);
	}
	return 0;
}

static int lua_pixel2(lua_State *L){
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if ((argc != 3) && (argc != 4)) return luaL_error(L, "wrong number of arguments");
	#endif
	int x = luaL_checkinteger(L,1);
	int y = luaL_checkinteger(L,2);
	int screen = luaL_checkinteger(L,3);
	int side=0;
	if (argc == 4) side = luaL_checkinteger(L,4);
	#ifndef SKIP_ERROR_HANDLING
		if ((x < 0) || (y < 0)) return luaL_error(L, "out of bounds");
		if ((screen == 0) && (x > 400)) return luaL_error(L, "out of framebuffer bounds");
		if ((screen == 1) && (x > 320)) return luaL_error(L, "out of framebuffer bounds");
		if ((screen <= 1) && (y > 240)) return luaL_error(L, "out of framebuffer bounds");
	#endif
	if (screen > 1){
		drawWarning("Warning: ","Skipped target image checks.\n");
		lua_pushinteger(L,GetImagePixel(x,y,(SDL_Surface*)screen));
	}else{
		uint8_t* buffer;
		if (screen == 0) buffer = TopLFB;
		else buffer = BottomLFB;
		lua_pushinteger(L,GetPixel(x,y,buffer,side));
	}
	return 1;
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

static int lua_color(lua_State *L) {
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if ((argc != 3) && (argc != 4)) return luaL_error(L, "wrong number of arguments");
	#endif
    int r = luaL_checkinteger(L, 1);
    int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 3);
	int a = 255;
	if (argc==4) a = luaL_checkinteger(L, 4);
    u32 color = r | (g << 8) | (b << 16) | (a << 24);
    lua_pushinteger(L,color);
    return 1;
}

static int lua_getR(lua_State *L) {
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
    int color = luaL_checkinteger(L, 1);
    u32 colour = color & 0xFF;
    lua_pushinteger(L,colour);
    return 1;
}

static int lua_getG(lua_State *L) {
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
    int color = luaL_checkinteger(L, 1);
    u32 colour = (color >> 8) & 0xFF;
    lua_pushinteger(L,colour);
    return 1;
}

static int lua_getB(lua_State *L) {
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
    int color = luaL_checkinteger(L, 1);
    u32 colour = (color >> 16) & 0xFF;
    lua_pushinteger(L,colour);
    return 1;
}

static int lua_getA(lua_State *L) {
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
    int color = luaL_checkinteger(L, 1);
    u32 colour = (color >> 24) & 0xFF;
    lua_pushinteger(L,colour);
    return 1;
}

static int lua_vblank(lua_State *L){
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	return 0;
}

static int lua_clearScreen(lua_State *L){
    int argc = lua_gettop(L);
    #ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	int screen = luaL_checkinteger(L,1);
	#ifndef SKIP_ERROR_HANDLING
		if ((screen != 1) && (screen != 0)) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	clearScreen(screen);
	return 0;
}

//Register our Console Functions
static const luaL_Reg Console_functions[] = {
  {0, 0}
};

//Register our Color Functions
static const luaL_Reg Color_functions[] = {
  {"new",                				lua_color},
  {"getR",								lua_getR},
  {"getG",								lua_getG},
  {"getB",								lua_getB},
  {"getA",								lua_getA},
  {0, 0}
};

//Register our Screen Functions
static const luaL_Reg Screen_functions[] = {
  {"loadImage",			lua_loadimg},
  {"drawImage",			lua_drawimg},
  {"drawPartialImage",  lua_partial}, 
  {"freeImage",			lua_freeimg},
  {"debugPrint",		lua_print},
  {"drawPixel",			lua_pixel},
  {"getPixel",			lua_pixel2},
  {"flip",				lua_flip},
  {"refresh",			lua_refresh},
  {"waitVblankStart",	lua_vblank},
  {"clear",  			lua_clearScreen}, 
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