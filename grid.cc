#include <iostream>
#include "util.hh"
#include <string>
#include "projection.hh"
#include "grid.hh"

object model = object::fromFile("models/hex.obj");
object grid[2*view * 2*view];
int x,y;

object& get(int x, int y) {
  while (x < 0) x += view;
  while (y < 0) y += view;
  x %= 2*view;
  y %= 2*view;
  return grid[x + y*2*view];
}

/*
void show() {
  for (int y = 0; y < 2*view; y++) {
    for (int x = 0; x < 2*view; x++) {
      if (get(x,y)) std::cout << "#";
      else std::cout << ".";
    }
    std::cout << std::endl;
  }
}

int main() {
  for (int i = 0; i < 2*view; i++) {
    for (int j = 0; j < 2*view; j++) {
      get(i,j) = 0;  
    }
  }

  show();
  std::cout << std::endl;
  int x = 5, y = 5;
  get(x,y) = 1;
  int nx = 6, ny = 5;
  get(nx,ny) = 1;
  if (nx > x) {
    //where it was to where it is now
    for (int i = x+view; i < nx+view; i++) {
      for (int j = 0; j < 2*view; j++) {
        get(i,j) = 1;
      }
    }
  } else if (nx < x) {
    for (int i = x+view; i > nx+view; i--) {
      for (int j = 0; j < 2*view; j++) {
        get(i,j) = 1;
      }
    }
  }
  if (ny > y) {
    //where it was to where it is now
    for (int i = y+view; i < ny+view; i++) {
      for (int j = 0; j < 2*view; j++) {
        get(j,i) = 1;
      }
    }
  } else if (ny < y) {
    //where it was to where it is now
    for (int i = y+view; i > ny+view; i--) {
      for (int j = 0; j < 2*view; j++) {
        get(j,i) = 1;
      }
    }
  }

  show();
  return 0;
}
*/


void gridInit(screen& s, int tx, int ty) {
  x = tx;
  y = ty;
  model.scale(16);
  for (int y = 0; y < 2*view; y++) {
    for (int x = 0; x < 2*view; x++) {
      object& tmp = get(x,y);
      tmp = model;
      s.inScene.push_back(&tmp); //address of referenced object
      tmp.translate(v3(x,0,y) * GRIDSTEP);
    }
  }
}

void gridUpdate(int nx, int ny) {
  return;
  if (nx > x) {
//    //where it was to where it is now
    for (int i = x+view; i < nx+view; i++) {
      for (int j = 0; j < 2*view; j++) {
        object& tmp = get(i,j);
//        tmp = model;
        tmp.translate(v3(view,0,0)*GRIDSTEP);
      }
    }
  } 
  else if (nx < x) {
    for (int i = x+view; i > nx+view; i--) {
      for (int j = 0; j < 2*view; j++) {
        object& tmp = get(i,j);
        tmp = model;
        tmp.translate(v3(-1*view,0,0)*GRIDSTEP);
      }
    }
  }

//  if (ny > y) {
//    //where it was to where it is now
//    for (int i = y+view; i < ny+view; i++) {
//      for (int j = 0; j < 2*view; j++) {
//        object& tmp = get(i,j);
//        tmp = model;
//        tmp.translate(v3(0,0,ny-y) * GRIDSTEP);
//      }
//    }
//  } else if (ny < y) {
//    //where it was to where it is now
//    for (int i = y+view; i > ny+view; i--) {
//      for (int j = 0; j < 2*view; j++) {
//        object& tmp = get(i,j);
//        tmp = model;
//        get(i,j) = model;
//        tmp.translate(v3(0,0,ny-y) * GRIDSTEP);
//      }
//    }
//  }
  x = nx;
  y = ny;
}
