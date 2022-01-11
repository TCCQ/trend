#include "util.hh"
#include "projection.hh"
//#include "sdlfb.hh"
#include "fb.hh"
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include <chrono>
#include <fcntl.h>
#include <termios.h>
#include "timage.hh"
#include "matrix.hh"
#include "tpool.hh"
#include <thread>

#define WORKTHREADS 8

const struct timespec mil = {0,16666666};
bool quit = false;
//SDL_Window* win;
std::string texture;
timage curText;
tpool threadPool;

void doInput(screen& s, char e) {
  if (e == 'w') {
    s.inScene[0]->translate(v3(10,0,0));
  } else if (e == 's') {
    s.inScene[0]->translate(v3(-10,0,0));
  } else if (e == 'q') {
    s.inScene[0]->translate(v3(0,10,0));
  } else if (e == 'e') {
    s.inScene[0]->translate(v3(0,-10,0));
  } else if (e == 'a') {
    s.inScene[0]->translate(v3(0,0,-10));
  } else if (e == 'd') {
    s.inScene[0]->translate(v3(0,0,10));
  } else if (e == 'r') {
    s.inScene[0]->scale(1.5);
  } else if (e == 'f') {
    s.inScene[0]->scale(1/1.5);
  } else if (e == 'z') {
    s.inScene[0]->rotateZ(0.05);
  } else if (e == 'x') {
    s.inScene[0]->rotateX(0.05);
  } else if (e == 'c') {
    s.inScene[0]->rotateY(0.05);
  } else if (e == '1') {
    s.rotX(0.05);
  } else if (e == '2') {
    s.rotY(0.05);
  } else if (e == '3') {
    s.rotZ(0.05);
  } else if (e == 'i') {
    v3 sNorm = s.yUnitOrth.cross(s.xUnitOrth);
    if (sNorm.dot(v3(0,1,0)) < 0.99) {
      s.rotArb(s.xUnitOrth, -0.05);
    }
  } else if (e == 'k') {
    v3 sNorm = s.yUnitOrth.cross(s.xUnitOrth);
    if (sNorm.dot(v3(0,1,0)) > -0.99) {
      s.rotArb(s.xUnitOrth, 0.05);
    }
  } else if (e == 'j') {
    s.rotY(-0.05);
  } else if (e == 'l') {
    s.rotY(0.05);
  } else if (e == '/') {
    quit = true;
  }
//    std::cout << s.toString() << std::endl <<std::endl;
}

/*
//void mouse (SDL_MouseMotionEvent& e) {
//  std::cout << e.x << ", " << e.y << std::endl;
//  std::cout << "(" << e.xrel << ", " << e.yrel << ")" << std::endl;
//  if (e.state & SDL_BUTTON_LMASK) {
//    //horizontaln
//    s.rotY(-2*M_PI * (((float)e.xrel) / SWIDTH)); 
//
//    //vertical
//    v3 sNorm = s.yUnitOrth.cross(s.xUnitOrth);
//    float ndy = sNorm.dot(v3(0,1,0));
//    if (e.yrel > 0) {
//      if (ndy < 0.99) {
//        s.rotArb(s.xUnitOrth, -2*M_PI * (((float)e.yrel) / SHEIGHT));
//      }
//    } else if (e.yrel < 0) {
//      if (ndy > -0.99) {
//        s.rotArb(s.xUnitOrth, -2*M_PI * (((float)e.yrel) / SHEIGHT));
//      }
//    }
//  }
//}
*/

bool direction = false;
void frame(int sig, screen& s) {
  char e;
  while (read(0, &e, 1) != -1) {
    doInput(s, e);
  }
  //TODO consider multithreading the screenclear
  s.initFrame(); //clear screen and zbuff
//  s.inScene[0]->rotateY(0.5);
//  for (int y = 0; y <curText.height; y++) {
//    for (int x = 0; x < curText.width; x++) {
//      tcolor p = curText.get(x,y);
//      setPixel(x,y,p.r,p.g,p.b);
//    }
//  }
  s.renderScene();
}

struct termios old = {0};
void terminalReset() {
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  tcsetattr(0,TCSADRAIN, &old); //reset the terminal stuff
}

int main(int argc, char** argv) {
  std::string card = "/dev/dri/card0";
  if (argc >= 4) card = argv[3];
  if (fbSetup(card)) { //NEEDS to happen before s is init., cause this sets SWIDTH/SHEIGHT
    std::cout <<"failed fb setup" <<std::endl;
    fbCleanup();
    terminalReset();
    return 1;
  } 

  //start interal setup
  screen s = screen(SWIDTH,SHEIGHT);
  object doc;
  if (argc >= 3) {
    texture = std::string(argv[2]);
    curText = timage::fromFile(texture);
    doc = object::objAndTexture(std::string(argv[1]), texture);
    doc.scale(16);
    s.inScene.push_back(&doc);
  } else {
    fbCleanup();
    terminalReset();
    return 1;
  }
  s.lightLoc = v3(-500,0,500);
  s.spotStrength = 0.75;
  s.ambientStrength = 0.25;
  s.translate(v3(-1300,0,0));
  //end screen setup

  //terminal setup
  if (fcntl(0,F_SETFL, O_NONBLOCK) == -1) {
    std::cout << "couldn't set nonblocking input" <<std::endl;
    fbCleanup();
    terminalReset();
    return 1;
  }
  //(no echo, don't wait for newline)
  if (tcgetattr(0, &old) < 0) {
    std::cout << "couldn't get term attributes" << std::endl;
    fbCleanup();
    terminalReset();
    return 1;
  }
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSADRAIN, &old) < 0) {
    std::cout << "couldn't set term attributes" << std::endl;
    fbCleanup();
    return 1;
  }
  //end terminal setup


//  signal(SIGALRM, &frame);
//  ualarm(1,33333); //30 fps

  //establish threadpool
  for (int t = 0; t < WORKTHREADS; t++) {
    std::thread tn = std::thread(&tpool::poolWork, &threadPool);
    tn.detach();
  }
  //pool is filled with threads

  clock_t start,end;
  while (!quit) {
    start = clock();
    frame(SIGALRM, s);
    end = clock();
    std::cout << end-start << ", " << CLOCKS_PER_SEC<< std::endl;
  }

  //cleanup
  terminalReset();
  fbCleanup();
}
