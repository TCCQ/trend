#include "projection.hh"
//util is included in .hh
//<vector> is in util.hh
#include "sdlfb.hh"
#include <SDL2/SDL.h>
#include <iostream>
#include <limits>

//returns v3 relative to world origin 
//intersection of projection line and screen plane
//no guarantees that the vertex is not behind the plane or anything else
v3 screen::insectLoc (const v3 vertex) {
  const v3& p = pjtLoc;
  const v3& sm = scrnSlope; //slope for screen
  const v3& so = scrnLoc; //offset for screen
  v3 w = vertex - p;
  
  //pjtpnt & vertex form line of the form
  //pjtpnt + t(vertex - pjtpnt)
  //we want to first find t by pluging into the screen plane function, that is
  //0 = a(x-x0) + b(y-y0) + c(z-z0)
  //so we algebra and find 
  //t = (ax0 + b0 + cz0 - apx - bpy - cpz) / (awx + bwy + cwz)
  //where p_ is the pjtpnt coords and w_ is the (vertex - pjtpnt) coords

  const float t = (sm.x*so.x + sm.y*so.y + sm.z*so.z - sm.x*p.x - sm.y*p.y - sm.z*p.z) / 
                  (sm.x*w.x + sm.y*w.y + sm.z*w.z);
  
  return v3(p.x + t*w.x, p.y + t*w.y, p.z + t*w.z);
}

//TODO remember to check for behind/between when using the insectLoc output
//call on point returned by insectLoc
pnt screen::locOnScreen (const v3 intersect) {
  const v3 w = intersect - scrnLoc; //adjust to be relative to center of screen
  pnt out;
  out.x = w.dot(xUnitOrth); 
  out.y = w.dot(yUnitOrth);
  out *= 10;
  return out;
}

v3 screen::pixTov3(int x, int y) {
  x -= SWIDTH/2; 
  y -= SHEIGHT/2; 
  return xUnitOrth*x + yUnitOrth*y + scrnLoc;
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

void screen::zbuffclear() {
  for (int h = 0; h < height; h++) {
    for (int w = 0; w < width; w++) {
      zbuff[w + width*h] = std::numeric_limits<float>().max();
    }
  }
}

bool screen::zbuffCheckV1 (int x, int y, object& obj, int planeIdx) {
//  zbuff[x + width*y] = std::numeric_limits<float>().max();
  v3 verScrn = pixTov3(x,y);
  v3 param = verScrn - pjtLoc; //param of line throug pix and pjtLoc

  v3 c[3];
  for (int i = 0; i < 3; i++) {
    c[i] = obj.vertexes[obj.planes[(planeIdx*3) + i]];
  }
  v3 norm = (c[1] - c[0]).cross(c[2] - c[0]); //polygon normal 

  float ndp = norm.dot(param); 
  if (ndp == 0) return false;
  float depth = (norm.dot(c[0]) - norm.dot(pjtLoc))/(ndp); //coef. for line
//  std::cerr << ((param * depth) + pjtLoc).toString() << std::endl;
//  depth = (param * depth).length();
//  std::cerr << std::numeric_limits<float>().max() << ", " << depth << ", " << zbuff[x + width*y] << std::endl;

  if( depth < zbuff[x + width*y]) {
    zbuff[x + width*y] = depth;
    return true;
  } else return false;
}

void screen::renderObj(object& obj) {
  zbuffclear();
  struct qval* tmpPlanes = new qval[obj.np];
  float average;
  for (unsigned int i = 0; i < obj.np; i++) {
    tmpPlanes[i].index = i;

    v3 midside = (obj.vertexes[obj.planes[(i*3) + 1]] + obj.vertexes[obj.planes[(i*3) + 2]] ) * 0.5F;
    //point in the middle of the line from second to third vertex
    midside = (midside - obj.vertexes[obj.planes[i*3]]) * (2.0f/3.0f);
    //now is the vec from first vertex to midpoint, scaled by 2/3
    tmpPlanes[i].value = (midside + obj.vertexes[obj.planes[i*3]]).distance(pjtLoc);
  }

  //tmpPlanes is now filled with data
//  sortqval(tmpPlanes, obj.np);
  //tmpPlanes is sorted low-high


  pnt* points = new pnt[obj.nv];
  //assume all the points are fine, see locOnScreen warning
  for (unsigned int i = 0; i < obj.nv; i++) {
    points[i] = locOnScreen(insectLoc(obj.vertexes[i] + obj.translation));
  }

  //TODO should I render the points too? consider 

  pnt curBound[3];
  float minx,miny,maxx,maxy;
  int tx,ty;
  for (int i = obj.np-1; i >= 0; i--) {
    int colorPlaneNumber = tmpPlanes[i].index;
//    v3 normal = obj.vertexes[obj.planes[colorPlaneNumber+1]] - obj.vertexes[obj.planes[colorPlaneNumber]];
//    normal = normal.cross(obj.vertexes[obj.planes[colorPlaneNumber+2]] - obj.vertexes[obj.planes[colorPlaneNumber]]);
    for (int j = 0; j < 3; j++) {
      curBound[j] = points[obj.planes[tmpPlanes[i].index * 3 + j]];
      //note that the indexes of obj.vertexes and points align
    }
    minx = (curBound[0].x < curBound[1].x)? curBound[0].x:curBound[1].x;
    minx = (minx < curBound[2].x)? minx:curBound[2].x;

    maxx = (curBound[0].x > curBound[1].x)? curBound[0].x:curBound[1].x;
    maxx = (maxx > curBound[2].x)? maxx:curBound[2].x;

    miny = (curBound[0].y < curBound[1].y)? curBound[0].y:curBound[1].y;
    miny = (miny < curBound[2].y)? miny:curBound[2].y;

    maxy = (curBound[0].y > curBound[1].y)? curBound[0].y:curBound[1].y;
    maxy = (maxy > curBound[2].y)? maxy:curBound[2].y;

    for (int x = minx; x < maxx; x++) {
      for (int y = miny; y < maxy; y++) {
        if (checkIfInside(pnt(x,y),curBound,3)) {
          tx = (int)SWIDTH/2 + x;
          ty = (int)SHEIGHT/2 + y;
          if (zbuffCheckV1(tx,ty,obj, colorPlaneNumber)) {
//            setPixel(sdlScreen, tx,ty, 0, colorPlaneNumber << 5, 0xFF - colorPlaneNumber << 5, 0xFF);
              setPixelFB(tx,ty,0,colorPlaneNumber << 5, 0xFF - colorPlaneNumber << 5, 0xFF);
          }
        }
      }
    }
  }
//  setPixel(sdlScreen,250,250,0xFF,0,0,0xFF);
  setPixelFB(250,250,0xFF,0,0,0xFF);
  delete tmpPlanes;
  delete points;
}