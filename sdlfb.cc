#include <SDL2/SDL.h>
#include "sdlfb.hh"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstdint>
#include <string.h>
#include <iostream>

SDL_Surface* sdlScreen;
char* fb;
int fbFilePtr;
int screenWidth;
int screenHeight;
//need to lock and unlock the surface before/after editing pixels
//should use maprgba and getrgba to avoid the hassle of pixel format stuff

//must lock manually before this operation
void setPixel(SDL_Surface* sdlScreen, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  if (x < 0 || x >= SWIDTH || y < 0 || y >= SHEIGHT) return;
  ((Uint32*)sdlScreen->pixels)[x + y*sdlScreen->pitch/4] = SDL_MapRGBA(sdlScreen->format, r,g,b,a);
}


bool initFB(int sw, int sh) {
  fb = NULL;
  screenWidth = sw;
  screenHeight = sh;
  //get bbp, width & height
  //TODO use macros for that
  const uint16_t fbw = 1920;
  const uint16_t fbh = 1080;
  const uint16_t bbp = 32;

  char str[10];
  int bbpfileptr = open("/sys/class/graphics/fb0/bits_per_pixel", O_RDONLY);
  if (bbpfileptr == -1) return false;
  str[read(bbpfileptr, str,2)] = 0;
  close(bbpfileptr);
  if (atoi(str) != 32) return false;

  int whfileptr = open("/sys/class/graphics/fb0/virtual_size", O_RDONLY);
  if (whfileptr == -1) return false;
  read(whfileptr, str, 10);
  str[4] = 0;
  str[9] = 0;
  close(whfileptr);
  if (atoi(str) != 1920 || atoi(str+5) != 1080) return false;
  //check against macros

  fbFilePtr = open("/dev/fb0", O_RDWR);
  if (fbFilePtr == -1) return false;

  fb = (char*)mmap(NULL, fbw*fbh*bbp/8, PROT_READ | PROT_WRITE, MAP_SHARED, fbFilePtr, 0);
  if (fb == MAP_FAILED) return false;


  return true;
}

void closeFB() {
  close(fbFilePtr);
}


//in BGRA form, checked by hand, don't see anywhere that is reported but /shrug
void setPixelFB(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  //TODO fbw is hardcoded here, fix
  uint32_t loc = (x*4) + (y * 1920*4);
  fb[loc] = b;
  fb[loc+1] = g;
  fb[loc+2] = r;
  fb[loc+3] = a; //has no effect
}
