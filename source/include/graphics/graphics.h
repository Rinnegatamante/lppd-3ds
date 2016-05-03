#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

extern uint8_t* TopLFB;
extern uint8_t* BottomLFB;
extern GLuint TopFB;
extern GLuint BottomFB;
extern GLenum texture_format;

void DrawPixel(uint8_t* screen,int x,int y,uint32_t color);
void DrawImagePixel(uint8_t* screen, int x, int y, uint32_t color, int w);
void DrawScreenText(int x, int y, char* str, uint32_t color,int screen,int side);
void DrawImageText(int x, int y, char* str, uint32_t color, SDL_Surface* img);
void drawTopFrame();
void drawBottomFrame();
void PrintScreenImage(int x,int y,SDL_Surface* file,int screen,int side);

void drawDebug(char* cmd, char* format, ...);
void drawError(char* cmd, char* format, ...);
void drawWarning(char* cmd, char* format, ...);
void drawCommand(char* cmd, char* format, ...);
#endif