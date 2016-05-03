#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "graphics.h"
#include "font.h"
#define u8 uint8_t
#define u32 uint32_t
#define u16 uint16_t

// Drawing functions using openGL
GLuint TopFB=NULL;
void drawTopFrame(){
	glClear( GL_COLOR_BUFFER_BIT );
	glBindTexture( GL_TEXTURE_2D, TopFB );
	glBegin( GL_QUADS );
	glTexCoord2i( 0, 0 );
	glVertex3f( 0, 0, 0 );
	glTexCoord2i( 1, 0 );
	glVertex3f( 400, 0, 0 );
	glTexCoord2i( 1, 1 );
	glVertex3f( 400, 240, 0 );
	glTexCoord2i( 0, 1 );
	glVertex3f( 0, 240, 0 );
	glEnd();
	glLoadIdentity();
}

GLuint BottomFB=NULL;
void drawBottomFrame(){
	glTranslatef(40.0,240.0,0.0);
	glBindTexture( GL_TEXTURE_2D, BottomFB );
	glBegin( GL_QUADS );
	glTexCoord2i( 0, 0 );
	glVertex3f( 0, 0, 0 );
	glTexCoord2i( 1, 0 );
	glVertex3f( 320, 0, 0 );
	glTexCoord2i( 1, 1 );
	glVertex3f( 320, 240, 0 );
	glTexCoord2i( 0, 1 );
	glVertex3f( 0, 240, 0 );
	glEnd();
	glLoadIdentity();
}

void DrawPixel(u8* screen, int x,int y, u32 color){
	int idx = (x + y * ((screen == TopLFB) ? 400 : 320)) * 3;
	*(u32*)&(screen[idx]) = (((color) & 0x00FFFFFF) | ((*(u32*)&(screen[idx])) & 0xFF000000));
}

void DrawScreenText(int x, int y, char* str, u32 color, int screen,int side){
	u8* buffer;
	u16 max_x = 320;
	if (screen == 0){
		if (side == 0) buffer = TopLFB;
		else return;
		max_x = 400;
	}else if (screen == 1) buffer = BottomLFB;
	unsigned short* ptr;
	unsigned short glyphsize;
	int i, cx, cy;
	for (i = 0; str[i] != '\0'; i++){
		if (str[i] < 0x21){
			x += 6;
			continue;
		}
		u16 ch = str[i];
		if (ch > 0x7E) ch = 0x7F;
		ptr = &font[(ch-0x20) << 4];
		glyphsize = ptr[0];
		if (!glyphsize){
			x += 6;
			continue;
		}
		x++;
		for (cy = 0; cy < 12; cy++){
			unsigned short val = ptr[4+cy];
			for (cx = 0; cx < glyphsize; cx++){
				if (val & (1 << cx)) DrawPixel(buffer, x+cx, y+cy, color);
			}
		}
		x += glyphsize;
		x++;
	}
}