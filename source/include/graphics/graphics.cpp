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
	if (x < 0 || y < 0 || y > 240 || ((screen == TopLFB) ? x > 400 : x > 320)) return;
	int idx = (x + y * ((screen == TopLFB) ? 400 : 320)) * 3;
	*(u32*)&(screen[idx]) = (((color) & 0x00FFFFFF) | ((*(u32*)&(screen[idx])) & 0xFF000000));
}

void DrawAlphaPixel(u8* screen, int x,int y, u32 color){
	if (x < 0 || y < 0 || y > 240 || ((screen == TopLFB) ? x > 400 : x > 320)) return;
	u8 alpha = (((color) >> 24) & 0xFF);
	int idx = (x + y * ((screen == TopLFB) ? 400 : 320));
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

void DrawScreenLine(int x0, int y0, int x1, int y1, u32 color, int screen, int side)
{
	u8* buffer = NULL;
	if (screen == 0){
		if (side == 0) buffer = TopLFB;
		else return;
	}else if (screen == 1) buffer = BottomLFB;
    int dy = y1 - y0;
    int dx = x1 - x0;
    int stepx, stepy;
   
    if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
    if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
    dy <<= 1;
    dx <<= 1;
   
    y0 *= 1;
    y1 *= 1;
    DrawPixel(buffer, x0, y0, color);
    if (dx > dy) {
        int fraction = dy - (dx >> 1);
        while (x0 != x1) {
            if (fraction >= 0) {
                y0 += stepy;
                fraction -= dx;
            }
            x0 += stepx;
            fraction += dy;
            DrawPixel(buffer, x0, y0, color);
        }
    } else {
        int fraction = dx - (dy >> 1);
        while (y0 != y1) {
            if (fraction >= 0) {
                x0 += stepx;
                fraction -= dy;
            }
            y0 += stepy;
            fraction += dx;
            DrawPixel(buffer, x0, y0, color);
        }
    }
}

void DrawAlphaScreenLine(int x0, int y0, int x1, int y1, u32 color, int screen, int side)
{
	u8* buffer = NULL;
	if (screen == 0){
		if (side == 0) buffer = TopLFB;
		else return;
	}else if (screen == 1) buffer = BottomLFB;
    int dy = y1 - y0;
    int dx = x1 - x0;
    int stepx, stepy;
   
    if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
    if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
    dy <<= 1;
    dx <<= 1;
   
    y0 *= 1;
    y1 *= 1;
    DrawAlphaPixel(buffer, x0, y0, color);
    if (dx > dy) {
        int fraction = dy - (dx >> 1);
        while (x0 != x1) {
            if (fraction >= 0) {
                y0 += stepy;
                fraction -= dx;
            }
            x0 += stepx;
            fraction += dy;
            DrawAlphaPixel(buffer, x0, y0, color);
        }
    } else {
        int fraction = dx - (dy >> 1);
        while (y0 != y1) {
            if (fraction >= 0) {
                x0 += stepx;
                fraction -= dy;
            }
            y0 += stepy;
            fraction += dx;
            DrawAlphaPixel(buffer, x0, y0, color);
        }
    }
}

void DrawImageLine(int x0, int y0, int x1, int y1, u32 color, SDL_Surface* img)
{
	void (*drawCallback)(u8*, int, int, u32, int);
	if (img->format->BitsPerPixel == 24) drawCallback = DrawImagePixel;
	else drawCallback = Draw32bppImagePixel;
	u8* buffer = (u8*)img->pixels;
    int dy = y1 - y0;
    int dx = x1 - x0;
    int stepx, stepy;
   
    if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
    if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
    dy <<= 1;
    dx <<= 1;
   
    y0 *= 1;
    y1 *= 1;
	drawCallback(buffer, x0, y0, color, img->w);
    if (dx > dy) {
        int fraction = dy - (dx >> 1);
        while (x0 != x1) {
            if (fraction >= 0) {
                y0 += stepy;
                fraction -= dx;
            }
            x0 += stepx;
            fraction += dy;
            drawCallback(buffer, x0, y0, color, img->w);
        }
    } else {
        int fraction = dx - (dy >> 1);
        while (y0 != y1) {
            if (fraction >= 0) {
                x0 += stepx;
                fraction -= dy;
            }
            y0 += stepy;
            fraction += dx;
            drawCallback(buffer, x0, y0, color, img->w);
        }
    }
}

void DrawImageRect(int x0, int y0, int x1, int y1, u32 color, SDL_Surface* img)
{	
	int y_size = 0;
	if (y1 > y0){
		int tmp = y1;
		y1 = y0;
		y0 = tmp;
	}
	y_size = y0 - y1;
	for (int i = 0;i < y_size; i++){
		DrawImageLine(x0,y1+i,x1,y1+i,color,img);
	}
}

void DrawRect(int x0, int y0, int x1, int y1, u32 color, int img, int side)
{	
	int y_size = 0;
	if (y1 > y0){
		int tmp = y1;
		y1 = y0;
		y0 = tmp;
	}
	y_size = y0 - y1;
	for (int i = 0;i < y_size; i++){
		DrawScreenLine(x0,y1+i,x1,y1+i,color,img,side);
	}
}

void DrawAlphaRect(int x0, int y0, int x1, int y1, u32 color, int img, int side)
{	
	int y_size = 0;
	if (y1 > y0){
		int tmp = y1;
		y1 = y0;
		y0 = tmp;
	}
	y_size = y0 - y1;
	for (int i = 0;i < y_size; i++){
		DrawAlphaScreenLine(x0,y1+i,x1,y1+i,color,img,side);
	}
}

void DrawImageRectEmpty(int x0, int y0, int x1, int y1, u32 color, SDL_Surface* img)
{	
	DrawImageLine(x0,y0,x0,y1,color,img);
	DrawImageLine(x0,y0,x1,y0,color,img);
	DrawImageLine(x1,y0,x1,y1,color,img);
	DrawImageLine(x0,y1,x1,y1,color,img);
}

void DrawRectEmpty(int x0, int y0, int x1, int y1, u32 color, int img, int side)
{	
	DrawScreenLine(x0,y0,x0,y1,color,img,side);
	DrawScreenLine(x0,y0,x1,y0,color,img,side);
	DrawScreenLine(x1,y0,x1,y1,color,img,side);
	DrawScreenLine(x0,y1,x1,y1,color,img,side);
}

void DrawAlphaRectEmpty(int x0, int y0, int x1, int y1, u32 color, int img, int side)
{	
	DrawAlphaScreenLine(x0,y0,x0,y1,color,img,side);
	DrawAlphaScreenLine(x0,y0,x1,y0,color,img,side);
	DrawAlphaScreenLine(x1,y0,x1,y1,color,img,side);
	DrawAlphaScreenLine(x0,y1,x1,y1,color,img,side);
}