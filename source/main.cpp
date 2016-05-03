#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_opengl.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "include/luaplayer.h"
#include "include/graphics/graphics.h"

uint8_t* TopLFB;
uint8_t* BottomLFB;
GLenum texture_format=NULL;
bool quit = 0;

int main(int argc, char* argv[]){
	
	// Checking arguments
	const char* errMsg;
	char filename[256];
	if (argc > 1) sprintf(filename,"%s",argv[1]);
	else sprintf(filename,"index.lua");
	
	// Initializing SDL and openGL stuffs
	SDL_Event event;
	SDL_Surface* screen = NULL;
	SDL_Init( SDL_INIT_EVERYTHING );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	screen = SDL_SetVideoMode( 400, 480, 32, SDL_OPENGL );
	glClearColor( 0, 0, 0, 0 );
	glEnable( GL_TEXTURE_2D );
	glViewport( 0, 0, 400, 480 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, 400, 480, 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	SDL_WM_SetCaption("Lua Player Plus Debugger 3DS", NULL);
	char* buffer;

	// Generating framebuffers for openGL
	TopLFB = (uint8_t*)malloc(3*400*240);
	BottomLFB = (uint8_t*)malloc(3*320*240);
	memset(TopLFB, 0, 3*400*240);
	memset(BottomLFB, 0, 3*320*240);
	texture_format=GL_RGB;
	glGenTextures( 1, &TopFB );
	glBindTexture( GL_TEXTURE_2D, TopFB );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, 3, 400, 240, 0, texture_format, GL_UNSIGNED_BYTE, TopLFB );
	glGenTextures( 1, &BottomFB );
	glBindTexture( GL_TEXTURE_2D, BottomFB );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, 3, 320, 240, 0, texture_format, GL_UNSIGNED_BYTE, BottomLFB );
	
	FILE* main_script = fopen(filename, "r");
	if (main_script == NULL){
		printf("FATAL ERROR: Main script not found.\n");
		quit = true;
	}else{
		fseek(main_script, 0, SEEK_END);
		uint32_t size = ftell(main_script);
		buffer = (char*)malloc(size+1);
		fseek(main_script, 0, SEEK_SET);
		fread(buffer, 1, size, main_script);
		fclose(main_script);
		buffer[size] = 0;
	}
	
	// Main loop
	while(!quit){
		while( SDL_PollEvent( &event ) ) {
			if( event.type == SDL_QUIT ) {
				quit = true;
			} 
		}
		
		if (buffer != NULL) errMsg = runScript((const char*)buffer, true);
		else{
			printf("FATAL ERROR: Main script is empty.\n");
			quit = true;
		}
		
		// Fake error to force interpreter shutdown
		if (strstr(errMsg, "lpp_exit_04")) break;
		
		while (!quit){
			while( SDL_PollEvent( &event ) ) {
				if( event.type == SDL_QUIT ) {
					quit = true;
				} 
			}
			SDL_GL_SwapBuffers();
		}
		
	}
	
	// Freeing everything
	free(buffer);
	free(BottomLFB);
	free(TopLFB);
	SDL_Quit(); 
	
	return 0;
}