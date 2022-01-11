#include "projection.hh"
//util is included in .hh
//<vector> is in util.hh
//#include "sdlfb.hh"
#include "fb.hh"
#include <SDL2/SDL.h>
#include <iostream>
#include <limits>
#include "timage.hh"
#include "matrix.hh"
#include <math.h>
#include "tpool.hh"
#include <thread>

extern tpool threadPool; //decalred for real in main, don't want to have to pass back and forth

//returns v3 relative to world origin 
//intersection of projection line and screen plane
//no guarantees that the vertex is not behind the plane or anything else
v3 screen::insectLoc (const v3 vertex) {
  const v3& p = pjtLoc;
  const v3& sm = scrnSlope; //slope for screen
  const v3& so = scrnLoc; //offset for screen
  v3 w = vertex - p;
  
  //pjtLoc & vertex form line of the form
  //pjtLoc + t(vertex - pjtLoc)
  //we want to first find t by plugging into the screen plane function, that is
  //0 = a(x-x0) + b(y-y0) + c(z-z0)
  //so we algebra and find 
  //t = (ax0 + by0 + cz0 - apx - bpy - cpz) / (awx + bwy + cwz)
  //where p_ is the pjtLoc coords and w_ is the (vertex - pjtLoc) coords

  const float t = sm.dot(so - p)/ sm.dot(w);
//  std::cout << t << std::endl;
  
  return p + (w * t);
}

//call on point returned by insectLoc
v2 screen::locOnScreen (const v3 intersect) {
  const v3 w = intersect - scrnLoc; //adjust to be relative to center of screen
  v2 out;
  out.x = w.dot(xUnitOrth); 
  out.y = w.dot(yUnitOrth);
  return out;
}

v3 screen::pixTov3(int x, int y) {
  x -= SWIDTH/2; 
  y -= SHEIGHT/2; 
  return (xUnitOrth*x) + (yUnitOrth*y) + scrnLoc;
}


struct qval {
  float value;
  int index;
};

//low to high merge sort
void sortqval (struct qval* arr, int size) {
  if (size == 1) return;
  else if (size == 2) {
    struct qval hold = arr[0];
    if (hold.value <= arr[1].value) return;
    else {
      arr[0] = arr[1];
      arr[1] = hold;
      return;
    }
  } else {
    const int pivot = size / 2;
    sortqval(arr, pivot);
    sortqval(arr+pivot, size-pivot);
    struct qval merge[size];
    int out = 0, ai = 0, bi = 0;
    while (out < size) {
      if (ai == pivot) {
        while (bi < size-pivot) {
          merge[out++] = (arr+pivot)[bi++];
        }
      } else if (bi == size-pivot) {
        while (ai < pivot) {
          merge[out++] = arr[ai++];
        }
      } else {
        if (arr[ai].value < (arr+pivot)[bi].value) merge[out++] = arr[ai++];
        else merge[out++] = (arr+pivot)[bi++];
      }
    }
    for (int i = 0; i < size; i++) {
      arr[i] = merge[i];
    }
  }
}

const float FMAX = std::numeric_limits<float>().max();

void screen::zbuffclear() {
  for (int h = 0; h < height; h++) {
    for (int w = 0; w < width; w++) {
      zbuff[w + width*h] = FMAX;
    }
  }
}

float screen::zbuffCheckV1 (int x, int y, const object& obj, int planeIdx) {
//  zbuff[x + width*y] = std::numeric_limits<float>().max();
  v3 verScrn = pixTov3(x,y);
  v3 param = verScrn - pjtLoc; //param of line throug pix and pjtLoc

  v3 c[3];
  for (int i = 0; i < 3; i++) {
    c[i] = obj.vertexes[obj.planes[(planeIdx*3) + i]];
  }
  v3 norm = (c[1] - c[0]).cross(c[2] - c[0]); //polygon normal 

  float ndp = norm.dot(param); 
  if (ndp == 0) return -1;
  float depth =  (norm.dot(c[0]) - norm.dot(pjtLoc))/(ndp); //coef. for line

  if(depth > 1 && depth < zbuff[x + width*y]) {
    zbuff[x + width*y] = depth;
    return depth;
  }
  return -1;
}

#define MAX(a,b) ((a<b)? b:a)
#define MIN(a,b) ((a>b)? b:a)

void screen::polygonRender(const object& obj, const v2* points, int i) {
  v2 curBound[3];
  float minx,miny,maxx,maxy;
  m33 textureLocator, textureLocatorInverse;
  const v3& a = obj.vertexes[obj.planes[i*3]];
  const v3& b = obj.vertexes[obj.planes[i*3 + 1]];
  const v3& c = obj.vertexes[obj.planes[i*3 + 2]];
  const v3 norm = (b-a).cross(c-a).normalize();
  
  v2 ta,tb,tc; //not a reference like before, but idk
  if (!obj.UVs.empty()) {
    ta = obj.UVs[obj.planeUVs[i*3]]; //corresponds to a/b/c
    tb = obj.UVs[obj.planeUVs[i*3 + 1]]; 
    tc = obj.UVs[obj.planeUVs[i*3 + 2]]; 
  }

  /*
  std::cout << ta.toString() << std::endl <<
    tb.toString() << std::endl <<
    tc.toString() <<std::endl <<std::endl;
  */

  if (norm.dot(a - pjtLoc) > 0) return; //was continue, now in lambda
  //back face culling

  for (int j = 0; j < 3; j++) {
    curBound[j] = points[obj.planes[(i*3)+j]];
    //note that the indexes of obj.vertexes and points align
    curBound[j].x += (int)SWIDTH/2;
    curBound[j].y += (int)SHEIGHT/2;
  }

  //go from coords relative to ploy to std coords
  textureLocator.setCol(0,b-a);
  textureLocator.setCol(1,c-a);
  textureLocator.setCol(2,norm);
  textureLocatorInverse = textureLocator.getInverse();

  minx = (curBound[0].x < curBound[1].x)? curBound[0].x:curBound[1].x;
  minx = (minx < curBound[2].x)? minx:curBound[2].x;

  maxx = (curBound[0].x > curBound[1].x)? curBound[0].x:curBound[1].x;
  maxx = (maxx > curBound[2].x)? maxx:curBound[2].x;

  miny = (curBound[0].y < curBound[1].y)? curBound[0].y:curBound[1].y;
  miny = (miny < curBound[2].y)? miny:curBound[2].y;

  maxy = (curBound[0].y > curBound[1].y)? curBound[0].y:curBound[1].y;
  maxy = (maxy > curBound[2].y)? maxy:curBound[2].y;

//    std::cerr << "(" << minx << ", " << maxx << ") (" << miny << ", " << maxy << ")" << std::endl;
  //clip rectangle to screen, allow for min>max, cause the loop will catch that
  minx = (minx > 0)? minx:0;
  miny = (miny > 0)? miny:0;
  maxx = (maxx < width-1)? maxx:width-1;
  maxy = (maxy < height-1)? maxy:height-1;

//    std::cout << "even considered? ";
//    std::cout << width << "," << height << std::endl;
  for (int x = minx; x < maxx; x++) {
    for (int y = miny; y < maxy; y++) {
//        std::cout << "possible pixel" << std::endl;
      if (v2(x,y).checkIfInside(curBound)) {
        locks[x + y*width].lock();
        float depth = zbuffCheckV1(x,y,obj, i);
        if (depth > 0) {
//            std::cout << "passed depth" << std::endl;
          //start light level
          v3 toPoly = ((((pixTov3(x,y)-pjtLoc) * depth) + pjtLoc) - lightLoc).normalize();
          //start texture
          v3 onPoly = (((pixTov3(x,y)-pjtLoc) * depth) + pjtLoc); //location on polygon

          onPoly -= a; //relative to one vertex
          
          tcolor tPixel;
          if (!obj.UVs.empty()) {
//              std::cout << onPoly.toString() << std::endl;
            v3 solution = textureLocatorInverse.multLeft(onPoly);
//              std::cout << solution.toString() << std::endl << std::endl;
            v2 triangleLoc = v2(solution.x, solution.y);
            v2 textureLoc = ((tb-ta) * triangleLoc.x) + ((tc-ta) * triangleLoc.y) + ta;
            textureLoc.x *= obj.texture.width;
            textureLoc.y *= obj.texture.height;
            tPixel = obj.texture.get((int)textureLoc.x, (int)textureLoc.y);
          } else {
            tPixel = tcolor(0xff,0xff,0xff); //no texture, just light
          }
          //start lighting
          float LL = toPoly.dot(norm)*256;
          LL = (LL > 0)? 0 : LL; //prevent lighting through the object / reverse side
          LL = log2(abs(LL));
          LL = (LL < 0)? 0 : LL;
          LL *= 32;
          LL /= 256;
          //got spotlight
          LL *= spotStrength;
          LL += ambientStrength; 
          LL = (LL > 1)? 1:LL;
          //apply lighting
          tPixel *= LL;
//            setPixel(sdlScreen, x,y, tPixel.r, tPixel.g, tPixel.b, 0xFF);
          setPixel(x,y, tPixel.r, tPixel.g, tPixel.b);
        }
        locks[x + y*width].unlock();
      }
    }
  }
}

void screen::renderObj(object& obj) {
  v2* points = new v2[obj.nv];
  //assume all the points are fine, see locOnScreen warning
  for (unsigned int i = 0; i < obj.nv; i++) {
    points[i] = locOnScreen(insectLoc(obj.vertexes[i]));
  }
  //TODO should I render the points too? consider 

  for (int i = 0; i< obj.np; i++) {
    //let threads do polygons, cause order doesn't matter
    //as long as zbuff and pixel setting are atomic (can't be interupted)
    std::function<void(void)> func = [=, this, &obj](){this->polygonRender(obj,points,i);};
    threadPool.enqueue(func);
  }
  //all work has been pushed to the queue
  //wait for all work to be done (frame ends)
  threadPool.waitUntilEmpty();
}

void screen::initFrame() {
  flip(); //work on back buffer
  zbuffclear();
  for (int y = 0; y < SHEIGHT; y++) {
    for (int x = 0; x < SWIDTH; x++) {
      setPixel(x,y,0x44,0x0,0xAA);
    }
  }
//  SDL_FillRect(sdlScreen, NULL, 0xFFFF);
}

void screen::renderScene() {
  for (int i =0; i < inScene.size(); i++) {
    renderObj(*(inScene[i]));
  }
}

void screen::translate(v3 t) {
  pjtLoc += t;
  scrnLoc += t;
}

void screen::scaleFocalLength(float s) {
  v3 diff = scrnLoc - pjtLoc;
  diff *= s;
  scrnLoc = pjtLoc + diff;
}

void screen::rotX(float theta) {
  pjtLoc.rotX(theta);
  scrnLoc.rotX(theta);
  scrnSlope.rotX(theta);
  xUnitOrth.rotX(theta);
  yUnitOrth.rotX(theta);
  lightLoc.rotX(theta);
}

void screen::rotY(float theta) {
  pjtLoc.rotY(theta);
  scrnLoc.rotY(theta);
  scrnSlope.rotY(theta);
  xUnitOrth.rotY(theta);
  yUnitOrth.rotY(theta);
  lightLoc.rotY(theta);
}

void screen::rotZ(float theta) {
  pjtLoc.rotZ(theta);
  scrnLoc.rotZ(theta);
  scrnSlope.rotZ(theta);
  xUnitOrth.rotZ(theta);
  yUnitOrth.rotZ(theta);
  lightLoc.rotZ(theta);
}

void screen::rotArb(v3 ray, float theta) {
  //ray should be normalized
  v3 tmp = ray;
  tmp.x=0; 
  tmp.normalize();
  //project onto YZ plane
  float first = acos(tmp.dot(v3(0,0,1)));
  this->rotX(first);
  ray.rotX(first);
  //aligned ray and cam to XZ plane
  float second = acos(ray.dot(v3(1,0,0)));
  this->rotY(second);
//  ray.rotY(second); //we don't need to keep track of ray anymore
  this->rotX(theta);
  //do cam rotation around X
  //then shift back to where it was
  this->rotY(-1 * second);
  this->rotX(-1 * first);
}

std::string screen::toString() {
  std::string out = "";
  out += "scrnLoc: " + scrnLoc.toString() + '\n';
  out += "pjtLoc: " + pjtLoc.toString() + '\n';
  out += "scrnSlope: " + scrnSlope.toString() +'\n';
  out += "xUnitOrth: " + xUnitOrth.toString() + '\n';
  out += "yUnitOrth: " + yUnitOrth.toString();
  return out;
}
