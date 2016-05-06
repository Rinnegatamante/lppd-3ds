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
#-----------------------------------------------------------------------------------------------------------------------#
#- Credits : -----------------------------------------------------------------------------------------------------------#
#-----------------------------------------------------------------------------------------------------------------------#
#- Smealum for ctrulib and ftpony src ----------------------------------------------------------------------------------#
#- StapleButter for debug font -----------------------------------------------------------------------------------------#
#- Lode Vandevenne for lodepng -----------------------------------------------------------------------------------------#
#- Jean-loup Gailly and Mark Adler for zlib ----------------------------------------------------------------------------#
#- Special thanks to Aurelio for testing, bug-fixing and various help with codes and implementations -------------------#
#-----------------------------------------------------------------------------------------------------------------------*/

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "include/luaplayer.h"
#include "include/graphics/graphics.h"
#include "include/utils.h"
#define stringify(str) #str
#define VariableRegister(lua, value) do { lua_pushinteger(lua, value); lua_setglobal (lua, stringify(value)); } while(0)
#define MAX_RAM_ALLOCATION	10485760
#define u8 uint8_t
#define u32 uint32_t
#define u64 uint64_t
#include <time.h>
static int lua_exit(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	drawCommand("System.exit: ", "Closing interpreter.\n");
	char string[20];
	strcpy(string,"lpp_exit_0456432");
	luaL_dostring(L, "collectgarbage()");
	return luaL_error(L, string); // NOTE: This is a fake error
}

static int lua_reboot(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	drawCommand("System.reboot: ", "Closing interpreter.\n");
	char string[20];
	strcpy(string,"lpp_exit_0456432");
	luaL_dostring(L, "collectgarbage()");
	return luaL_error(L, string); // NOTE: This is a fake error
}

static int lua_launch(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 2) return luaL_error(L, "wrong number of arguments");
	#endif
	const char* path = luaL_checkstring(L, 1);
	u8 mediatype = luaL_checkinteger(L, 2);
	drawCommand("System.launch3DSX: ", "Closing interpreter.\n");
	char string[20];
	strcpy(string,"lpp_exit_0456432");
	luaL_dostring(L, "collectgarbage()");
	return luaL_error(L, string); // NOTE: This is a fake error
}

static int lua_launchcia(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 2) return luaL_error(L, "wrong number of arguments");
	#endif
	u32 unique_id = luaL_checkinteger(L,1);
	u32 mediatype = luaL_checkinteger(L,2);
	drawCommand("System.launchCIA: ", "Closing interpreter.\n");
	char string[20];
	strcpy(string,"lpp_exit_0456432");
	luaL_dostring(L, "collectgarbage()");
	return luaL_error(L, string); // NOTE: This is a fake error
}

static int lua_launcharm9(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 2) return luaL_error(L, "wrong number of arguments");
	#endif
	const char* file = luaL_checkstring(L, 1);
	u32 offset = luaL_checkinteger(L, 2);
	drawCommand("System.launchPayload: ", "Closing interpreter.\n");
	char string[20];
	strcpy(string,"lpp_exit_0456432");
	luaL_dostring(L, "collectgarbage()");
	return luaL_error(L, string); // NOTE: This is a fake error
}

static int lua_installcia(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 2) return luaL_error(L, "wrong number of arguments");
	#endif
	const char* path = luaL_checkstring(L, 1);
	u8 mediatype = luaL_checkinteger(L, 2);
	drawWarning("Warning: ", "Skipping System.installCia function.\n");
	return 0;
}

static int lua_getFW(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	drawCommand("System.getFirmware: ", "Returning 2.50-9 FIRM version.\n");
	lua_pushinteger(L,2);
	lua_pushinteger(L,50);
	lua_pushinteger(L,9);
	return 3;
}

static int lua_getK(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	drawCommand("System.getKernel: ", "Returning 2.50-9 NATIVE_FIRM version.\n");
	lua_pushinteger(L,2);
	lua_pushinteger(L,50);
	lua_pushinteger(L,9);
	return 3;
}

static int lua_curdir(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0 && argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	if (argc == 0){
		char dir[1024];
		getcwd(dir, sizeof(dir));
		char res[1024];
		sprintf(res,"%s/",dir);
		lua_pushstring(L, res);
		return 1;
	}else{
		const char* path = luaL_checkstring(L, 1);
		chdir(path);
		return 0;
	}
}

static int lua_exists(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	const char* path = luaL_checkstring(L, 1);
	FILE* tmp = fopen(path,"r");
	if (tmp != NULL){
		fclose(tmp);
		lua_pushboolean(L, true);
	}else lua_pushboolean(L, false);
	return 1;
}

static int lua_checkbuild(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	drawCommand("System.checkBuild: ", "Returning CFW mode.\n");
	lua_pushinteger(L,1);
	return 1;
}

static int lua_getbattery(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	drawCommand("System.getBatteryLife: ", "Returning 100% state.\n");
	lua_pushinteger(L,4);
	return 1;
}

static int lua_ischarge(lua_State *L)
{
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	drawCommand("System.isBatteryCharging: ", "Returning false.\n");
	lua_pushboolean(L, false);
	return 1;
}

static int lua_gettime(lua_State *L){
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	u64 tot = time(NULL) % 86400;
	u8 hours = tot / 3600;
	u8 minutes = (tot % 3600) / 60;
	u8 seconds = tot % 60;
	lua_pushinteger(L,hours);
	lua_pushinteger(L,minutes);
	lua_pushinteger(L,seconds);
	return 3;
}

static int lua_getdate(lua_State *L){
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	lua_pushinteger(L,timeinfo->tm_wday + 1);
	lua_pushinteger(L,timeinfo->tm_mday);
	lua_pushinteger(L,timeinfo->tm_mon + 1);
	lua_pushinteger(L,timeinfo->tm_year + 1900);
	return 4;
}

//Register our System Functions
static const luaL_Reg System_functions[] = {
	{"getFirmware",			lua_getFW},
	{"getKernel",			lua_getK},
	{"launch3DSX",			lua_launch},
	{"launchCIA",			lua_launchcia},
	{"launchPayload",		lua_launcharm9},
	{"installCIA",			lua_installcia},
	{"exit",				lua_exit},
	{"reboot",				lua_reboot},
	{"currentDirectory",	lua_curdir},
	{"getTime",				lua_gettime},
	{"getDate",				lua_getdate},
	{"doesFileExist",		lua_exists},
	{"checkBuild",			lua_checkbuild},
	{0, 0}
};

void luaSystem_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, System_functions, 0);
	lua_setglobal(L, "System");
	uint8_t FREAD = 0;
	uint8_t FWRITE = 1;
	uint8_t FCREATE = 2;
	uint8_t NAND = 0;
	uint8_t SDMC = 1;
	VariableRegister(L,FREAD);
	VariableRegister(L,FWRITE);
	VariableRegister(L,FCREATE);
	VariableRegister(L,NAND);
	VariableRegister(L,SDMC);
}
