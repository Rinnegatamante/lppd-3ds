#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

extern uint8_t* TopLFB;
extern uint8_t* BottomLFB;
extern GLuint TopFB;
extern GLuint BottomFB;
extern GLenum texture_format;

void clearScreen(int screen);
void DrawPixel(uint8_t* screen,int x,int y,uint32_t color);
void DrawAlphaPixel(uint8_t* screen, int x,int y, uint32_t color);
void DrawAlphaScreenLine(int x0, int y0, int x1, int y1, uint32_t color, int screen, int side);
void DrawImageLine(int x0, int y0, int x1, int y1, uint32_t color, SDL_Surface* img);
void DrawScreenLine(int x0, int y0, int x1, int y1, uint32_t color, int screen, int side);
void DrawAlphaRect(int x0, int y0, int x1, int y1, uint32_t color, int screen, int side);
void DrawImageRect(int x0, int y0, int x1, int y1, uint32_t color, SDL_Surface* img);
void DrawRect(int x0, int y0, int x1, int y1, uint32_t color, int screen, int side);
void DrawAlphaRectEmpty(int x0, int y0, int x1, int y1, uint32_t color, int screen, int side);
void DrawImageRectEmpty(int x0, int y0, int x1, int y1, uint32_t color, SDL_Surface* img);
void DrawRectEmpty(int x0, int y0, int x1, int y1, uint32_t color, int screen, int side);
void DrawImagePixel(uint8_t* screen, int x, int y, uint32_t color, int w);
void Draw32bppImagePixel(uint8_t* screen, int x,int y, uint32_t color, int w);
void DrawScreenText(int x, int y, char* str, uint32_t color,int screen,int side);
void DrawImageText(int x, int y, char* str, uint32_t color, SDL_Surface* img);
void drawTopFrame();
void drawBottomFrame();
void PrintScreenImage(int x,int y,SDL_Surface* file,int screen,int side);
void PrintPartialScreenImage(int xp,int yp,int st_x,int st_y,int width,int height, SDL_Surface* result,int screen,int side);
void PrintPartialImageImage(int xp,int yp,int st_x,int st_y,int width,int height, SDL_Surface* result, SDL_Surface* result2);
uint32_t GetPixel(int x,int y,uint8_t* screen,int side);
uint32_t GetImagePixel(int x,int y,SDL_Surface* screen);

void drawDebug(char* cmd, char* format, ...);
void drawError(char* cmd, char* format, ...);
void drawWarning(char* cmd, char* format, ...);
void drawCommand(char* cmd, char* format, ...);
#endif