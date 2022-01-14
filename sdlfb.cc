#include <SDL2/SDL.h>
#include "fb.hh"
#include <string>

SDL_Surface* sdlScreen;
SDL_Window* win;
unsigned int SWIDTH = 800; 
unsigned int SHEIGHT = 800;

//need to lock and unlock the surface before/after editing pixels
//should use maprgba and getrgba to avoid the hassle of pixel format stuff

//must lock manually before this operation
void setPixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b) {
  if (x < 0 || x >= SWIDTH || y < 0 || y >= SHEIGHT) return;
  ((Uint32*)sdlScreen->pixels)[x + y*sdlScreen->pitch/4] = SDL_MapRGBA(sdlScreen->format, r,g,b,0xFF);
}

int fbSetup(std::string ignored) {
  SDL_Init(SDL_INIT_VIDEO);
  win = SDL_CreateWindow("trend", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SWIDTH, SHEIGHT, 0);
  sdlScreen = SDL_GetWindowSurface(win);
  SDL_LockSurface(sdlScreen);
  return 0;
}

void fbCleanup(void) {
  SDL_DestroyWindow(win);
  SDL_Quit();
}

void flip(void) {
  SDL_UnlockSurface(sdlScreen);
  SDL_UpdateWindowSurface(win);
  SDL_LockSurface(sdlScreen);
}
