#include <iostream>
#include "util.hh"
#include "projection.hh"
#include "sdlfb.hh"
#include <time.h>
#include <math.h>

//const struct timespec mil = {0,16666666};
const struct timespec mil = {0,16666666};
const float RTOT = sqrt(2)/2;
const float ffu = 0.577350269;

//extern SDL_Surface* sdlScreen;
int main() {
  if (initFB(SWIDTH,SHEIGHT)) {
    std::cout << "worked?" << std::endl;
    fb[0] = 0xFF;
    closeFB();
  } else {
    std::cout << "failed" << std::endl;
  }
//  return 0;

//  for (int x = 0; x < 1920; x++) {
//    for (int y = 0; y < 1080; y++) {
//      setPixelFB(x,y,0xFF,0,0,0xFF);
//    }
//  }
//  return 0;

//  pnt* t = new pnt[3];
//  t[0] = {-1,-1};
//  t[1] = {1,1};
//  t[2] = {-1,1};
//  checkIfInside({0,0}, t, 3);
//  return 0;

  screen s = screen(SWIDTH, SHEIGHT);
//  s.pjtLoc = v3(0,-10,0);
//  s.scrnLoc = v3(ffu, -10+ffu, ffu);
//  s.scrnSlope = v3(1,1,1);
//  s.yUnitOrth = v3(ffu, -1*ffu, ffu);
//  s.xUnitOrth = v3(ffu, ffu, -1*ffu);
  s.pjtLoc = v3(-10,0,0);
  s.scrnLoc = v3(-5,0,0);


  object test = object(
      { v3(10,10,10),v3(10,10,-10),v3(-10,10,-10),v3(-10,10,10),
        v3(10,-10,10),v3(10,-10,-10),v3(-10,-10,-10),v3(-10,-10,10) },
      { {0,1,2}, {0,2,3},
        {4,5,7}, {7,5,6},
        {2,6,7}, {7,2,3},
        {0,1,4}, {1,4,5},
        {4,7,3}, {4,3,0},
        {1,2,5}, {2,5,6}}
      );
  test.translation = v3(10,0,0);


//  SDL_Init(SDL_INIT_VIDEO);
//  SDL_Window* win = SDL_CreateWindow("trend", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SWIDTH, SHEIGHT, 0);
  //500x500 window
//  sdlScreen = SDL_GetWindowSurface(win);

//  SDL_FillRect(sdlScreen, NULL, 0xFFFF);


//  SDL_LockSurface(sdlScreen);
//  s.recalcObjPlaneOrder(test);
//  s.renderObj(test);
//  SDL_UnlockSurface(sdlScreen);
//  SDL_UpdateWindowSurface(win);



  int counter = 0;
  signed char xinc = 1;
  signed char yinc = 0;
//  SDL_Event event;
  bool quit = false;

  while (!quit) {
    //do input handling
//    while (SDL_PollEvent(&event)) {
//      switch (event.type) {
//        case SDL_QUIT:
//          quit=true;
//          break;
//        default:
//          break;
//      }
//    }

    //update coors
    s.pjtLoc += v3(0, yinc*0.1, xinc*0.1);
    s.scrnLoc += v3(0, yinc*0.1,xinc*0.1);
    ++counter;
    if (counter ==0) {xinc = 1; yinc = 0;}
    else if (counter == 200) {xinc = 0; yinc = 1;}
    else if (counter == 400) {xinc = -1; yinc = 0;}
    else if (counter == 600) {xinc = 0; yinc = -1;}
    else if (counter == 800) {xinc = 1; yinc=0; counter = 0;}

    //clear screen
//    SDL_FillRect(sdlScreen, NULL, 0xFFFF);
    for (int y = 0; y < SHEIGHT; y++) {
      for (int x = 0; x < SWIDTH; x++) {
        setPixelFB(x,y,0xFF,0x33,0,0);
      }
    }

    //update screen 
//    SDL_LockSurface(sdlScreen);
    s.renderObj(test);
//    SDL_UnlockSurface(sdlScreen);
//    SDL_UpdateWindowSurface(win);
//    SDL_Delay(5000);

    nanosleep(&mil,NULL);
  }
//  SDL_DestroyWindow(win);
//  SDL_Quit();
}
