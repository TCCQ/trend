#include <SDL2/SDL.h>

#define SWIDTH 500
#define SHEIGHT 500

extern SDL_Surface* sdlScreen;
extern unsigned short* zbuf;

extern char* fb;
bool initFB(int,int);
void closeFB();

void setPixel(SDL_Surface*, int,int,Uint8,Uint8,Uint8,Uint8);
void setPixelFB(int,int,Uint8,Uint8,Uint8,Uint8);