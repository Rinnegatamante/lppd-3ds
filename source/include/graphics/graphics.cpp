#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "graphics.h"
#include "font.h"
#include "../scrutil/scrutil.h"
#define u8 uint8_t
#define u32 uint32_t
#define u16 uint16_t

extern scrAttributes attrs;

void drawDebug(char* cmd, char* format, ...){
	scrSetColors(scrCyan,attrs.paper);
	printf(cmd);
	va_list arg;
	va_start (arg, format);
	scrSetColors(scrWhite,attrs.paper);
	vfprintf (stdout, format, arg);
	va_end (arg);
}

void drawCommand(char* cmd, char* format, ...){
	scrSetColors(scrGreen,attrs.paper);
	printf(cmd);
	va_list arg;
	va_start (arg, format);
	scrSetColors(scrWhite,attrs.paper);
	vfprintf (stdout, format, arg);
	va_end (arg);
}

void clearScreen(int screen){
	if (screen==1) memset(BottomLFB,0,230400);
	else memset(TopLFB,0,288000);
}

void drawWarning(char* cmd, char* format, ...){
	scrSetColors(scrYellow,attrs.paper);
	printf(cmd);
	va_list arg;
	va_start (arg, format);
	scrSetColors(scrWhite,attrs.paper);
	vfprintf (stdout, format, arg);
	va_end (arg);
}

void drawError(char* cmd, char* format, ...){
	scrSetColors(scrRed,attrs.paper);
	printf(cmd);
	va_list arg;
	va_start (arg, format);
	scrSetColors(scrWhite,attrs.paper);
	vfprintf (stdout, format, arg);
	va_end (arg);
}

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

void DrawAlphaPixel(u8* screen, int x,int y, u32 color){
	u8 alpha = (((color) >> 24) & 0xFF);
	int idx = (x + y * ((screen == TopLFB) ? 400 : 320)) * 3;
	float ratio = alpha / 255.0f;
	screen[idx*3] = ((color & 0xFF) * ratio) + (screen[idx*3] * (1.0 - ratio));
	screen[idx*3+1] = ((((color) >> 8) & 0xFF) * ratio) + (screen[idx*3+1] * (1.0 - ratio));
	screen[idx*3+2] = ((((color) >> 16) & 0xFF) * ratio) + (screen[idx*3+2] * (1.0 - ratio));
}

void DrawImagePixel(u8* screen, int x,int y, u32 color, int w){
	int idx = (x + y * w) * 3;
	*(u32*)&(screen[idx]) = (((color) & 0x00FFFFFF) | ((*(u32*)&(screen[idx])) & 0xFF000000));
}

void Draw32bppImagePixel(u8* screen, int x,int y, u32 color, int w){
	int idx = (x + y * w) * 4;
	*(u32*)&(screen[idx]) = color;
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

void DrawImageText(int x, int y, char* str, u32 color, SDL_Surface* img){
	void (*drawCallback)(u8*, int, int, u32, int);
	if (img->format->BitsPerPixel == 24) drawCallback = DrawImagePixel;
	else drawCallback = Draw32bppImagePixel;
	u8* buffer = (u8*)img->pixels;
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
				if (val & (1 << cx)) drawCallback(buffer, x+cx, y+cy, color, img->w);
			}
		}
		x += glyphsize;
		x++;
	}
}

void PrintScreenImage(int xp,int yp, SDL_Surface* result, int screen, int side){
	#ifndef SKIP_ERROR_HANDLING
		if(!result) return;
	#endif
	u8* buffer = NULL;
	if (screen == 0){
		if (side == 0) buffer = TopLFB;
		else return;
	}else if (screen == 1) buffer = BottomLFB;
	int x, y;
	if (result->format->BitsPerPixel == 24){
		for (y = 0; y < result->h; y++){
			for (x = 0; x < result->w; x++){
				u8* pixels_data = (u8*)result->pixels;
				u32 color = *((u32*)&pixels_data[(x + (result->w * y)) * 3]);
				DrawPixel(buffer,xp+x,yp+y,color);
			}
		}
	}else{
		for (y = 0; y < result->h; y++){
			for (x = 0; x < result->w; x++){
				u8* pixels_data = (u8*)result->pixels;
				u32 color = *((u32*)&pixels_data[(x + (result->w * y)) * 4]);
				DrawAlphaPixel(buffer,xp+x,yp+y,color);
			}
		}
	}
}

void PrintPartialImageImage(int xp,int yp,int st_x,int st_y,int width,int height, SDL_Surface* result, SDL_Surface* result2){
	void (*drawCallback)(u8*, int, int, u32, int);
	if (result2->format->BitsPerPixel == 24) drawCallback = DrawImagePixel;
	else drawCallback = Draw32bppImagePixel;
	u8* buffer = (u8*)result2->pixels;
	int x, y;
	if (result->format->BitsPerPixel){
		for (y = st_y; y < st_y + height; y++){
			for (x = st_x; x < st_x + width; x++){
				u8* pixels_data = (u8*)result->pixels;
				u32 color = *((u32*)&pixels_data[(x + (result->w * y)) * 3]);
				drawCallback(buffer,xp+x-st_x,yp+y-st_y,color,result2->w);
			}
		}
	}else{
		for (y = st_y; y < st_y + height; y++){
			for (x = st_x; x < st_x + width; x++){
				u8* pixels_data = (u8*)result->pixels;
				u32 color = *((u32*)&pixels_data[(x + (result->w * y)) * 4]);
				drawCallback(buffer,xp+x-st_x,yp+y-st_y,color,result2->w);
			}			
		}
	}
}

void PrintPartialScreenImage(int xp,int yp,int st_x,int st_y,int width,int height, SDL_Surface* result,int screen,int side){
	#ifndef SKIP_ERROR_HANDLING
		if(!result) return;
	#endif
	u8* buffer = NULL;
	if (screen == 0){
		if (side == 0) buffer = TopLFB;
		else return;
	}else if (screen == 1) buffer = BottomLFB;
	int x, y;
	if (result->format->BitsPerPixel){
		for (y = st_y; y < st_y + height; y++){
			for (x = st_x; x < st_x + width; x++){
				u8* pixels_data = (u8*)result->pixels;
				u32 color = *((u32*)&pixels_data[(x + (result->w * y)) * 3]);
				DrawPixel(buffer,xp+x-st_x,yp+y-st_y,color);
			}
		}
	}else{
		for (y = st_y; y < st_y + height; y++){
			for (x = st_x; x < st_x + width; x++){
				u8* pixels_data = (u8*)result->pixels;
				u32 color = *((u32*)&pixels_data[(x + (result->w * y)) * 4]);
				DrawAlphaPixel(buffer,xp+x-st_x,yp+y-st_y,color);
			}			
		}
	}
}

u32 GetPixel(int x,int y,u8* screen,int side){
	int idx = (x + y * ((screen == TopLFB) ? 400 : 320)) * 3;
	u32 color = (screen[idx] & 0x00FFFFFF) | (0xFFFFFFFF & 0xFF000000);
	return color;
}

u32 GetImagePixel(int x,int y,SDL_Surface* screen){
	u32 color;
	if (screen->format->BitsPerPixel == 24){
		int idx = (x + y * screen->w) * 3;
		color = ((*(u32*)&(((u8*)screen->pixels)[idx])) & 0x00FFFFFF) | (0xFFFFFFFF & 0xFF000000);
	}else{ 
		int idx = (x + y * screen->w) * 4;
		color = *(u32*)&(((u8*)screen->pixels)[idx]);
	}
return color;
}