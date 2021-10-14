#include <iostream>
#include "util.hh"
#include "projection.hh"
#include "sdlfb.hh"
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <fstream>

const struct timespec mil = {0,16666666};
bool quit = false;
screen s = screen(SWIDTH, SHEIGHT);
SDL_Window* win;

void doInput(SDL_KeyboardEvent& e) {
  if (e.keysym.sym == SDLK_w) {
    s.inScene[0]->translate(v3(1,0,0));
  } else if (e.keysym.sym == SDLK_s) {
    s.inScene[0]->translate(v3(-1,0,0));
  } else if (e.keysym.sym == SDLK_q) {
    s.inScene[0]->translate(v3(0,1,0));
  } else if (e.keysym.sym == SDLK_e) {
    s.inScene[0]->translate(v3(0,-1,0));
  } else if (e.keysym.sym == SDLK_a) {
    s.inScene[0]->translate(v3(0,0,-1));
  } else if (e.keysym.sym == SDLK_d) {
    s.inScene[0]->translate(v3(0,0,1));
  } else if (e.keysym.sym == SDLK_r) {
    s.scaleFocalLength(2);
  } else if (e.keysym.sym == SDLK_f) {
    s.scaleFocalLength(0.5);
  }
}

void frame(int sig) {
  SDL_Event event;
  if (sig != SIGALRM) return;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        quit=true;
        ualarm(0,0);
        break;
      case SDL_KEYDOWN:
          doInput(event.key);
        break;
      default:
        break;
    }
  }

  //update screen 
  SDL_LockSurface(sdlScreen);
  s.initFrame(); //clear screen and zbuff
  s.renderScene();
  SDL_UnlockSurface(sdlScreen);
  SDL_UpdateWindowSurface(win);
  (s.inScene[0])->rotateY(0.05);
  (s.inScene[1])->rotateY(-0.05);
  (s.inScene[2])->rotateX(0.05);
  (s.inScene[3])->rotateX(-0.05);
  (s.inScene[4])->rotateZ(0.05);
//  (s.inScene[1])->rotateY(-0.05);
}

//extern SDL_Surface* sdlScreen;
int main() {

  s.pjtLoc = v3(-30,0,0);
  s.scrnLoc = v3(530,0,0);
  s.lightLoc = v3(-30,0,-10);

  object tetra = objectFromFile("tetra.obj");
  tetra.scale(10);
  tetra.translate(v3(10,-15,15));
  s.inScene.push_back(&tetra);

  object cube = objectFromFile("cube.obj");
  cube.scale(6);
  cube.translate(v3(10,15,15));
  s.inScene.push_back(&cube);

  object octo = objectFromFile("octo.obj");
  octo.scale(6);
  octo.translate(v3(10,15,-15));
  s.inScene.push_back(&octo);

  object dodec = objectFromFile("dodec.obj");
  dodec.scale(6);
  dodec.translate(v3(10,-15,-15));
  s.inScene.push_back(&dodec);

  object icos = objectFromFile("icos.obj");
  icos.scale(6);
  icos.translate(v3(10,0,0));
  s.inScene.push_back(&icos);

  SDL_Init(SDL_INIT_VIDEO);
  win = SDL_CreateWindow("trend", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SWIDTH, SHEIGHT, 0);
  sdlScreen = SDL_GetWindowSurface(win);

  SDL_FillRect(sdlScreen, NULL, 0xFFFF);

  signal(SIGALRM, &frame);
  ualarm(1,33333); //30 fps

  while (!quit) {
    //do input handling
    nanosleep(&mil,NULL);
  }
  SDL_DestroyWindow(win);
  SDL_Quit();
}
