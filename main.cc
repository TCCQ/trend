#include "util.hh"
#include "projection.hh"
#include "sdlfb.hh"
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include "timage.hh"
#include "matrix.hh"

const struct timespec mil = {0,16666666};
bool quit = false;
screen s = screen(SWIDTH, SHEIGHT);
SDL_Window* win;
std::string texture;
timage curText;

void doInput(SDL_KeyboardEvent& e) {
  if (e.keysym.sym == SDLK_w) {
    s.inScene[0]->translate(v3(10,0,0));
  } else if (e.keysym.sym == SDLK_s) {
    s.inScene[0]->translate(v3(-10,0,0));
  } else if (e.keysym.sym == SDLK_q) {
    s.inScene[0]->translate(v3(0,10,0));
  } else if (e.keysym.sym == SDLK_e) {
    s.inScene[0]->translate(v3(0,-10,0));
  } else if (e.keysym.sym == SDLK_a) {
    s.inScene[0]->translate(v3(0,0,-10));
  } else if (e.keysym.sym == SDLK_d) {
    s.inScene[0]->translate(v3(0,0,10));
  } else if (e.keysym.sym == SDLK_r) {
    s.inScene[0]->scale(1.5);
  } else if (e.keysym.sym == SDLK_f) {
    s.inScene[0]->scale(1/1.5);
  } else if (e.keysym.sym == SDLK_z) {
    s.inScene[0]->rotateZ(0.05);
  } else if (e.keysym.sym == SDLK_x) {
    s.inScene[0]->rotateX(0.05);
  } else if (e.keysym.sym == SDLK_c) {
    s.inScene[0]->rotateY(0.05);
  } else if (e.keysym.sym == SDLK_1) {
    s.rotX(0.05);
  } else if (e.keysym.sym == SDLK_2) {
    s.rotY(0.05);
  } else if (e.keysym.sym == SDLK_3) {
    s.rotZ(0.05);
  } else if (e.keysym.sym == SDLK_i) {
    v3 sNorm = s.yUnitOrth.cross(s.xUnitOrth);
    if (sNorm.dot(v3(0,1,0)) < 0.99) {
      s.rotArb(s.xUnitOrth, -0.05);
    }
  } else if (e.keysym.sym == SDLK_k) {
    v3 sNorm = s.yUnitOrth.cross(s.xUnitOrth);
    if (sNorm.dot(v3(0,1,0)) > -0.99) {
      s.rotArb(s.xUnitOrth, 0.05);
    }
  } else if (e.keysym.sym == SDLK_j) {
    s.rotY(-0.05);
  } else if (e.keysym.sym == SDLK_l) {
    s.rotY(0.05);
  }
//    std::cout << s.toString() << std::endl <<std::endl;
}

void mouse (SDL_MouseMotionEvent& e) {
//  std::cout << e.x << ", " << e.y << std::endl;
//  std::cout << "(" << e.xrel << ", " << e.yrel << ")" << std::endl;
  if (e.state & SDL_BUTTON_LMASK) {
    //horizontaln
    s.rotY(-2*M_PI * (((float)e.xrel) / SWIDTH)); 

    //vertical
    v3 sNorm = s.yUnitOrth.cross(s.xUnitOrth);
    float ndy = sNorm.dot(v3(0,1,0));
    if (e.yrel > 0) {
      if (ndy < 0.99) {
        s.rotArb(s.xUnitOrth, -2*M_PI * (((float)e.yrel) / SHEIGHT));
      }
    } else if (e.yrel < 0) {
      if (ndy > -0.99) {
        s.rotArb(s.xUnitOrth, -2*M_PI * (((float)e.yrel) / SHEIGHT));
      }
    }
  }
}

bool direction = false;
void frame(int sig) {
  SDL_Event event;
  if (sig != SIGALRM) return;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        quit=true;
        ualarm(0,0);
        break;
      case SDL_MOUSEMOTION:
        mouse(event.motion);
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
  for (int y = 0; y <curText.height; y++) {
    for (int x = 0; x < curText.width; x++) {
      tcolor p = curText.get(x,y);
      setPixel(sdlScreen,x,y,p.r,p.g,p.b,0xff);
    }
  }
  s.renderScene();
  SDL_UnlockSurface(sdlScreen);
  SDL_UpdateWindowSurface(win);
//  (s.inScene[0])->rotateY(0.05);
}

int main(int argc, char** argv) {
  object doc;
  if (argc >= 3) {
    texture = std::string(argv[2]);
    curText = timage::fromFile(texture);
    doc = object::objAndTexture(std::string(argv[1]), texture);
    doc.scale(16);
    doc.translate(v3(0,-200,0));
    s.inScene.push_back(&doc);
  } 
  s.lightLoc = v3(-500,0,500);
  s.spotStrength = 0.0;
  s.ambientStrength = 1.;

  s.translate(v3(-1300,0,0));
//  object first = object::objAndTexture(models[0], texture);
//  object test = first;
//  test.rotateY(M_PI/2);
//  test.rotateZ(M_PI/2);
//  test.scale(10);
//  s.inScene.push_back(&test);

  SDL_Init(SDL_INIT_VIDEO);
  win = SDL_CreateWindow("trend", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SWIDTH, SHEIGHT, 0);
  sdlScreen = SDL_GetWindowSurface(win);

  SDL_FillRect(sdlScreen, NULL, 0xFFFF);

//  frame(SIGALRM);
  signal(SIGALRM, &frame);
  ualarm(1,33333); //30 fps

  while (!quit) {
    nanosleep(&mil,NULL);
  }
  SDL_DestroyWindow(win);
  SDL_Quit();
}
