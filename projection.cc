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

const float FMAX = std::numeric_limits<float>().max();

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
//  std::cout << "t: " << t << std::endl;
  if (t < 0) return v3(FMAX,0,0); //error code, maybe should restructure, but this should be fine
  
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

//basically the inverse of the above two
v3 screen::pixTov3(int x, int y) {
  x -= SWIDTH/2; 
  y -= SHEIGHT/2; 
  return (xUnitOrth*x) + (yUnitOrth*y) + scrnLoc;
}


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

  v3 c[3]; //could maybe be a reference, but compiler should fix that
  for (int i = 0; i < 3; i++) {
    c[i] = obj.vertexes[obj.planes[(planeIdx*3) + i]];
  }
  v3 norm = (c[1] - c[0]).cross(c[2] - c[0]); //polygon normal 

  float ndp = norm.dot(param); 
  if (ndp == 0) return -1;
  float depth = (norm.dot(c[0]) - norm.dot(pjtLoc))/(ndp); //coef. for line

  if(depth > 1 && depth < zbuff[x + width*y]) {
    zbuff[x + width*y] = depth;
    return depth;
  }
  return -1;
}

#define MAX(a,b) ((a<b)? b:a)
#define MIN(a,b) ((a>b)? b:a)

void screen::polygonRender(const object& obj, const v2* points, const int i) {
  v2 curBound[3];
  float minx,miny,maxx,maxy;
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

  //back face culling
  if (norm.dot(a - pjtLoc) >= 0) return; //was continue, now in lambda

  for (int j = 0; j < 3; j++) {
    curBound[j] = points[obj.planes[(i*3)+j]];
    if (curBound[j].x == FMAX && curBound[j].y == 0) {
      //error code, skip this one
      return;
    }
    /*
     * TODO make this slightly more refined for now I will just skip the
     * entirety of any polygon that has a vertex that is projected backwards eg
     * it is behind the projection point with respect to the screen for large
     * polygons (think a ground tile) that stretch for a long distance in one
     * or more directions, could cause problems because it will remove the
     * parts of the polygon that are correctly rendered in front
     *
     * no great ideas on how to fix this, but know that it is a problem this
     * starts in the insectLoc function, and the error code there is passed to
     * here
     */
    //note that the indexes of obj.vertexes and points align
//    curBound[j].x += (int)SWIDTH/2;
//    curBound[j].y += (int)SHEIGHT/2;
    //reminder that these are now centered, not absolute
  }
  //pv verson is plus-minus relative to center of screen
  v2 pva = curBound[0];
  v2 pvb = curBound[1];
  v2 pvc = curBound[2];
  //curbound version is absolute + relateive to top left
  for (int i = 0; i < 3; i++) {
    curBound[i].x += (int)(SWIDTH/2);
    curBound[i].y += (int)(SHEIGHT/2);
  }

  /*
   * So my basic idea is to project the polygon verticies on to the scree
   * (see curBound above)
   * and then for the pixels inside (latice point inside arb non-zero area triangle TODO better)
   * express them in terms of the vectors along the edges of the triangle (in the screen plane)
   * and then use coordinates of that R^2 basis with the matching edges of the polygon in R^3
   * which should give me the point on the polygon that matches the picture
   * this way I should get the correct point on the polygon even if it is at a weird angle
   * (possilby excluding if it is perp. to the screen plane, but that should get caught by backface culling)
   * Then I can do further tests to make sure the point is really something I want to render
   */

  //start basis change matrix
  m33 triangleRelativeConverter, trcInverse;
  triangleRelativeConverter.setCol(0, v3(pvb - pva)); //convert from v2 to v3, z = 0
  triangleRelativeConverter.setCol(1, v3(pvc - pva));
  triangleRelativeConverter.setCol(2, v3(0,0,1)); //dummy, no vec perp to screen, makes invertible
  trcInverse = triangleRelativeConverter.getInverse();
  //now multiplying on the right by a v3(pixel - pva) will give a v3 relative to the triangle verticies
  m33 polygonRelativeToGlobal;
  polygonRelativeToGlobal.setCol(0,b-a);
  polygonRelativeToGlobal.setCol(1,c-a);
  polygonRelativeToGlobal.setCol(2,norm); //is normalized, hopefully won't cause problems
  //muliplying on right by tri-rel. coords will give global coords

  //end of matrix setup



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

  //don't like using auto, but as far as I can tell it is unavoidable here
  auto pixel = [&] (int x, int y) {
        locks[x + y*width].lock();
//        float depth = zbuffCheckV1(x,y,obj, i);
        v2 pixelRelToCenter = v2(x - (int)(SWIDTH/2), y - (int)(SHEIGHT/2));
        pixelRelToCenter -= pva; //make relative to projected a

        v3 triRelCoords = trcInverse.multLeft(v3(pixelRelToCenter));  //vector is on the right
        v3 onPoly = polygonRelativeToGlobal.multLeft(triRelCoords) + a; //global std basis coords of pnt on poly

        float depth = onPoly.distance(pjtLoc);
//        std::cout << "D = " << depth << std::endl;
        if (depth < zbuff[x + y*width]) {
          zbuff[x + y*width] = depth;
//            std::cout << "passed depth" << std::endl;
          //start light level
          v3 toPoly = (onPoly - lightLoc).normalize(); //from light to polygon

//          if ((onPoly - pjtLoc).dot(scrnLoc - pjtLoc) <= 0) {
//            continue;
//          }

          //skip pixels that are projected from somewhere that is not in front of the screen
//          if ((onPoly-pjtLoc).normalize().dot(scrnLoc - pjtLoc) < ((pixTov3(0,0) - pjtLoc).normalize()).dot(scrnLoc - pjtLoc)) {
//            locks[x + y*width].unlock();
//            continue;
//          }
          /*
           * if the angle between the 3space point and the center of the screen,
           * both relative to the projection point,
           * is greater than (the dot product is smaller or negative)
           * the angle between the top left corner of the screen and the center of the screen,
           * again relative to the projection point,
           *
           * then it is safe to skip the pixel
           * 
           */
          

          //start texture
//          onPoly -= a; //relative to one vertex
          
          tcolor tPixel;
          if (!obj.UVs.empty()) {
//              std::cout << onPoly.toString() << std::endl;
//            v3 solution = textureLocatorInverse.multLeft(onPoly);
//              std::cout << solution.toString() << std::endl << std::endl;
            v2 triangleLoc = v2(triRelCoords.x, triRelCoords.y);
            v2 textureLoc = ((tb-ta) * triangleLoc.x) + ((tc-ta) * triangleLoc.y) + ta;
            textureLoc.x *= obj.texture.width;
            textureLoc.y *= obj.texture.height;
            tPixel = obj.texture.get((int)textureLoc.x, (int)textureLoc.y);
            //TODO make sure this still works
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

  };

  for (int x = minx; x < maxx; x++) {
    for (int y = miny; y < maxy; y++) {

      if (v2(x,y).checkIfInside(curBound)) {
        pixel(x,y);
      }
    }
  }
}

void screen::renderObj(object& obj) {
  v2* points = new v2[obj.nv];
  //assume all the points are fine, see locOnScreen warning
  for (unsigned int i = 0; i < obj.nv; i++) {
    v3 hold = insectLoc(obj.vertexes[i]);
    if (hold.x == FMAX && hold.y == 0 && hold.z == 0) {
      //this is an error code that this point is projected backwards
      //and thus should not be rendered
      //pass error code forward
      points[i] = v2(FMAX,0); 
    } else {
      points[i] = locOnScreen(hold);
    }
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
  delete [] points;
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
  currentLoc += t;
}

void screen::scaleFocalLength(float s) {
  v3 diff = scrnLoc - pjtLoc;
  diff *= s;
  scrnLoc = pjtLoc + diff;
}

void screen::rotX(float theta) {
  v3 hold = currentLoc;
  translate(hold*-1);
  pjtLoc.rotX(theta);
  scrnLoc.rotX(theta);
  scrnSlope.rotX(theta);
  xUnitOrth.rotX(theta);
  yUnitOrth.rotX(theta);
//  lightLoc.rotX(theta);
  translate(hold);
}

void screen::rotY(float theta) {
  v3 hold = currentLoc;
  translate(hold*-1);
  pjtLoc.rotY(theta);
  scrnLoc.rotY(theta);
  scrnSlope.rotY(theta);
  xUnitOrth.rotY(theta);
  yUnitOrth.rotY(theta);
//  lightLoc.rotY(theta);
  translate(hold);
}

void screen::rotZ(float theta) {
  v3 hold = currentLoc;
  translate(hold*-1);
  pjtLoc.rotZ(theta);
  scrnLoc.rotZ(theta);
  scrnSlope.rotZ(theta);
  xUnitOrth.rotZ(theta);
  yUnitOrth.rotZ(theta);
//  lightLoc.rotZ(theta);
  translate(hold);
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
