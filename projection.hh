#include "util.hh"
#include <vector>

//be very careful editing any of these values
//I might wanna add some protections for those
#ifndef screenClass
#define screenClass
class screen {
  private:
    void zbuffclear();
    float zbuffCheckV1(int, int,object&,int);    
  public:
    screen(int w, int h) : width(w), height(h){
      zbuff = new float[w*h];
      pjtLoc = v3(0,0,0);
      scrnLoc = v3(1,0,0);
      scrnSlope = v3(1,0,0);
      xUnitOrth = v3(0,0,1);
      yUnitOrth = v3(0,1,0);
      lightLoc = v3(0,0,0);
    }

    ~screen() {
      delete zbuff;
    }

    //funcs to generate the stuff on screen
    v3 insectLoc(const v3);
    v2 locOnScreen(const v3);
    v3 pixTov3(int,int);
    void renderObj(object&);
    void renderScene();
    void initFrame();
    void translate(v3);
    void scaleFocalLength(float);

    const int width, height;
    float* zbuff;

    std::vector<object*> inScene;

    v3 lightLoc;
    //TODO rework to use pjtLoc, normal vector, and fixed FOV scalar.
    v3 pjtLoc; //x,y,z of projection point
    //next two are screen plane
    v3 scrnLoc; //x0,y0,z0 in below
    v3 scrnSlope; //a,b,c in a(x-x0) + b(y-y0) + c(z-z0) = 0
    v3 xUnitOrth;
    v3 yUnitOrth;//these should be perp. to eachother and to the normal of the screen
    //normal vector can be found with x cross y
    //note that that will point toward the projection point, not outward
};
#endif
