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

static int lua_openwav(lua_State *L)
{

	// Init function
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if ((argc != 1) && (argc != 2)) return luaL_error(L, "wrong number of arguments");
	#endif
	const char *file_tbo = luaL_checkstring(L, 1);
	// Check for Streaming feature usage
	bool mem_size = false;
	if (argc == 2) mem_size = lua_toboolean(L, 2);
	
	// Open file
	FILE* fileHandle = fopen(file_tbo, "rb");
	
	// Check for magic
	u32 next_pos,pos,magic,samplerate,bytesRead,jump,chunk=0x00000000;
	u16 audiotype;
	fread(&magic, 1, 4, fileHandle);
	if (magic == 0x46464952){
	
		// Init wav struct
		Music* songFile = (Music*)malloc(sizeof(Music));
		songFile->ch = 0xDEADBEEF;
		strcpy(songFile->author,"");
		strcpy(songFile->title,"");
		u64 size;
		songFile->big_endian = false;
		
		// Metadatas extraction
		fseek(fileHandle, 16, SEEK_SET);
		fread(&jump, 1, 4, fileHandle);
		fseek(fileHandle, jump, SEEK_CUR);
		while (chunk != 0x61746164){
			fread(&chunk, 1, 4, fileHandle);
	
			//Chunk LIST detection
			if (chunk == 0x5453494C){
				u32 chunk_size;
				u32 subchunk;
				u32 subchunk_size;
				fread(&chunk_size, 1, 4, fileHandle);
				next_pos = ftell(fileHandle) + chunk_size;
				u32 sub_pos = ftell(fileHandle);
				fread(&subchunk, 1, 4, fileHandle);
				if (subchunk == 0x4F464E49){
					sub_pos = ftell(fileHandle);
					u32 start = sub_pos;
					while (sub_pos < (chunk_size + start)){
						fread(&subchunk, 1, 4, fileHandle);
						fread(&subchunk_size, 1, 4, fileHandle);
						if (subchunk == 0x54524149){
							char* author = (char*)malloc(subchunk_size);
							fread(author, 1, subchunk_size, fileHandle);
							strcpy(songFile->author,author);
							songFile->author[subchunk_size+1] = 0;
							free(author);
						}else if (subchunk == 0x4D414E49){
							char* title = (char*)malloc(subchunk_size);
							fread(title, 1, subchunk_size, fileHandle);
							strcpy(songFile->title,title);
							songFile->title[subchunk_size+1] = 0;
							free(title);
						}
						sub_pos = sub_pos + 8 + subchunk_size;
						u8 checksum;
						u32 before = ftell(fileHandle);
						fread(&checksum, 1, 1, fileHandle);
						if (checksum != 0) fseek(fileHandle, before, SEEK_SET); //Prevent errors switching subchunks
					}
				}
			}
			fseek(fileHandle, next_pos, SEEK_SET);
	
		}
		pos = ftell(fileHandle) + 8;

		// Music info extraction
		fseek(fileHandle, 1, SEEK_END);
		size = ftell(fileHandle);
		u16 raw_enc;
		fseek(fileHandle, 20, SEEK_SET);
		fread(&raw_enc, 1, 2, fileHandle);
		fread(&audiotype, 1, 2, fileHandle);
		fread(&samplerate, 1, 4, fileHandle);
		fseek(fileHandle, 32, SEEK_SET);
		fread(&(songFile->bytepersample), 1, 2, fileHandle);
		if (raw_enc == 0x01) songFile->encoding = CSND_ENCODING_PCM16;
		else if (raw_enc == 0x11) songFile->encoding = CSND_ENCODING_ADPCM;
		songFile->samplerate = samplerate;
		songFile->audiotype = audiotype;
		drawCommand("Sound.openWav: ","%s opened (samplerate: %i, %s)\n", file_tbo, songFile->samplerate, (songFile->audiotype == 1) ? "mono" : "stereo");
		songFile->isPlaying = false;
		songFile->size = size - pos;
		songFile->startRead = 0;
		songFile->mem_size = 0;
		songFile->sourceFile = NULL;
		songFile->sourceFile2 = NULL;
		if (mem_size){
			drawWarning("Warning: ","Remember to call Sound.updateStream() to sanitize the streaming in main loop on retail hardware.");
			songFile->sourceFile2 = Mix_LoadMUS(file_tbo);
		}else songFile->sourceFile = Mix_LoadWAV(file_tbo);
		
		// Pushing memory block to LUA stack
		songFile->magic = 0x4C534E44;
		lua_pushinteger(L,(u32)songFile);
		
	}
	
	// Closing file if no streaming feature usage
	fclose(fileHandle);
	
	return 1;
}

static int lua_openogg(lua_State *L)
{

	// Init function
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if ((argc != 1) && (argc != 2)) return luaL_error(L, "wrong number of arguments");
	#endif
	const char *file_tbo = luaL_checkstring(L, 1);
	// Check for Streaming feature usage
	bool mem_size = false;
	if (argc == 2) mem_size = lua_toboolean(L, 2);
	
	// Open file
	FILE* fileHandle = fopen(file_tbo, "rb");
	
	// Check for magic
	u32 next_pos,pos,magic,samplerate,bytesRead,jump,chunk=0x00000000;
	u16 audiotype;
	fread(&magic, 1, 4, fileHandle);
	if (magic == 0x5367674F){
	
		// Init wav struct
		Music* songFile = (Music*)malloc(sizeof(Music));
		songFile->ch = 0xDEADBEEF;
		strcpy(songFile->author,"");
		strcpy(songFile->title,"");
		u64 size;
		songFile->big_endian = false;
		
		// Metadatas extraction
		fseek(fileHandle, 0x60, SEEK_SET);
		u32 half_magic = 0xDEADBEEF;
		u32 info_size = 0xDEADBEEF;
		u32 offset;
		char info_type[7];
		int i = 0;
		while (half_magic != 0x726F7603){
			i++;
			fread(&half_magic,4,1,fileHandle);
			fseek(fileHandle,0x60+i, SEEK_SET);
		}
		fseek(fileHandle, 0x06, SEEK_CUR);
		fread(&info_size,4,1,fileHandle);
		fseek(fileHandle, info_size + 4, SEEK_CUR);
		fread(&info_size,4,1,fileHandle);
		while (info_size != 0x6F760501){
			offset = ftell(fileHandle);
			if (offset > 0x200) break; // Temporary patch for files without COMMENT section
			fread(&info_type,6,1,fileHandle);
			if ((strcmp((const char*)&info_type,"ARTIST") == 0) || (strcmp((const char*)&info_type,"artist") == 0)){
				fseek(fileHandle,0x01,SEEK_CUR);
				fread(&songFile->author,info_size - 7,1,fileHandle);
				songFile->author[info_size - 7] = 0;
			}else if ((strcmp((const char*)&info_type,"TITLE=") == 0) || (strcmp((const char*)&info_type,"title=") == 0)){
				fread(&songFile->title,info_size - 6,1,fileHandle);
				songFile->title[info_size - 6] = 0;
			}
			fseek(fileHandle,offset+info_size, SEEK_SET);
			fread(&info_size,4,1,fileHandle);
		}

		// Music info extraction
		//fseek(fileHandle, 1, SEEK_END);
		//size = ftell(fileHandle);
		fseek(fileHandle, 38, SEEK_SET);
		fread(&audiotype, 1, 2, fileHandle);
		fread(&samplerate, 1, 4, fileHandle);
		songFile->bytepersample = audiotype<<1;
		songFile->encoding = CSND_ENCODING_PCM16;
		songFile->samplerate = samplerate;
		songFile->audiotype = audiotype;
		drawCommand("Sound.openOgg: ","%s opened (samplerate: %i, %s)\n", file_tbo, songFile->samplerate, (songFile->audiotype == 1) ? "mono" : "stereo");
		songFile->isPlaying = false;
		//songFile->size = size - pos;
		songFile->startRead = 0;
		songFile->mem_size = 0;
		songFile->sourceFile = NULL;
		songFile->sourceFile2 = NULL;
		if (mem_size){
			drawWarning("Warning: ","Remember to call Sound.updateStream() to sanitize the streaming in main loop on retail hardware.");
			songFile->sourceFile2 = Mix_LoadMUS(file_tbo);
		}else songFile->sourceFile = Mix_LoadWAV(file_tbo);
		
		// Pushing memory block to LUA stack
		songFile->magic = 0x4C534E44;
		lua_pushinteger(L,(u32)songFile);
		
	}
	
	// Closing file if no streaming feature usage
	fclose(fileHandle);
	
	return 1;
}


static int lua_play(lua_State *L)
{
	
	// Init Function
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 2 && argc != 3)	return luaL_error(L, "wrong number of arguments");
	#endif
	Music* src = (Music*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C534E44) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	bool loop = lua_toboolean(L, 2);
	int interp = 0;
	if (argc == 3) interp = luaL_checkinteger(L, 3);
	
	// Selecting a free channel
	/*int ch = src->ch;
	if (ch == 0xDEADBEEF){
		ch = 0;
		while (audioChannels[ch]){
			ch++;
			if (ch >= 24) return luaL_error(L, "audio device is busy");
		}
		audioChannels[ch] = true;
	}
	src->ch = ch;*/
	
	// Playing audio file
	src->isPlaying = true;
	if (src->sourceFile == NULL && src->sourceFile2 == NULL){
		drawError("Error: ","Failed opening audio device.\n");
		drawError("SDL_Mixer Error: ", "%s\n",Mix_GetError());
	}else{
		if (src->sourceFile != NULL) src->ch = Mix_PlayChannel(-1,src->sourceFile, loop ? -1 : 0);
		else Mix_PlayMusic(src->sourceFile2, loop ? -1 : 0);
	}
	
	return 0;
}


static int lua_service(lua_State *L){
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	char srv[9];
	sprintf(srv,"csnd:SND");
	drawCommand("Sound.getService: ", "Returning csnd:SND service.\n");
	lua_pushstring(L, srv);
	return 1;
}

static int lua_soundinit(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	Mix_OpenAudio(44100,AUDIO_S16LSB,2,640);
	return 0;
}

static int lua_soundterm(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	Mix_CloseAudio();
	return 0;
}

static int lua_getTitle(lua_State *L){
int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	Music* src = (Music*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C534E44) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	lua_pushstring(L, src->title);
	return 1;
}

static int lua_getAuthor(lua_State *L){
int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	Music* src = (Music*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C534E44) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	lua_pushstring(L, src->author);
	return 1;
}

static int lua_getType(lua_State *L){
int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	Music* src = (Music*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C534E44) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	lua_pushinteger(L, src->audiotype);
	return 1;
}

static int lua_updatestream(lua_State *L){
	int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 0) return luaL_error(L, "wrong number of arguments");
	#endif
	return 0;
}

static int lua_getSrate(lua_State *L){
int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	Music* src = (Music*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C534E44) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	lua_pushinteger(L, src->samplerate);
	return 1;
}

static int lua_pause(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	Music* src = (Music*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C534E44) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	if (src->isPlaying){
		if (src->sourceFile != NULL) Mix_Pause(src->ch);
		else Mix_PauseMusic();
		src->isPlaying = false;
	}
	return 0;
}

static int lua_resume(lua_State *L)
{
    int argc = lua_gettop(L);
	#ifndef SKIP_ERROR_HANDLING
		if (argc != 1) return luaL_error(L, "wrong number of arguments");
	#endif
	Music* src = (Music*)luaL_checkinteger(L, 1);
	#ifndef SKIP_ERROR_HANDLING
		if (src->magic != 0x4C534E44) return luaL_error(L, "attempt to access wrong memory block type");
	#endif
	if (!src->isPlaying){
		if (src->sourceFile != NULL) Mix_Resume(src->ch);
		else Mix_ResumeMusic();
		src->isPlaying = true;	
	}
	return 0;
}

static const luaL_Reg Sound_functions[] = {
	{"getService",				lua_service},
	{"openWav",					lua_openwav},
	{"openOgg",					lua_openogg},
	{"init",					lua_soundinit},
	{"term",					lua_soundterm},
	{"getAuthor",				lua_getAuthor},
	{"getTitle",				lua_getTitle},
	{"getSrate",				lua_getSrate},
	{"getType",					lua_getType},
	{"play",					lua_play},
	{"pause",					lua_pause},
	{"resume",					lua_resume},
	{"updateStream",			lua_updatestream},
	{0, 0}
};

void luaSound_init(lua_State *L) {
	lua_newtable(L);
	luaL_setfuncs(L, Sound_functions, 0);
	lua_setglobal(L, "Sound");
	int LINEAR_INTERP = 0;
	int POLYPHASE_INTERP = 1;
	int NO_INTERP = 2;
	VariableRegister(L, LINEAR_INTERP);
	VariableRegister(L, POLYPHASE_INTERP);
	VariableRegister(L, NO_INTERP);
}