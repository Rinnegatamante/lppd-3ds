#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

extern uint8_t* TopLFB;
extern uint8_t* BottomLFB;
extern GLuint TopFB;
extern GLuint BottomFB;
extern GLenum texture_format;

void DrawPixel(uint8_t* screen,int x,int y,uint32_t color);
void DrawScreenText(int x, int y, char* str, uint32_t color,int screen,int side);
void drawTopFrame();
void drawBottomFrame();

#endif