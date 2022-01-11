#include <vector>
#include "util.hh"
#include <string>
#include <mutex>

//be very careful editing any of these values
//I might wanna add some protections for those
#ifndef screenClass
#define screenClass
class screen {
  private:
    void zbuffclear();
    float zbuffCheckV1(int, int, const object&,int);    
    void polygonRender(const object&, const v2*, int);
    std::mutex* locks;
  public:
    screen(int w, int h) : width(w), height(h){
      zbuff = new float[w*h];
      locks = new std::mutex[w*h];
      scrnLoc = v3(550,0,0);
      pjtLoc = v3(0,0,0);
      scrnSlope = v3(1,0,0);
      xUnitOrth = v3(0,0,1);
      yUnitOrth = v3(0,-1,0);
      lightLoc = v3(0,0,0);
      ambientStrength = 0.15;
      spotStrength = 0.75;
    }

    ~screen() {
      delete [] zbuff;
      delete [] locks;
    }

    //funcs to generate the stuff on screen
    v3 insectLoc(const v3);
    v2 locOnScreen(const v3);
    v3 pixTov3(int,int);
    void renderObj(object&);
    void renderScene();
    void initFrame();
    void translate(v3);
    void rotX(float);
    void rotY(float);
    void rotZ(float);
    void rotArb(v3, float);
    void scaleFocalLength(float);

    std::string toString();

    const int width, height;
    float* zbuff;

    std::vector<object*> inScene;

    v3 lightLoc;
    float spotStrength; //0-1
    float ambientStrength; //should be 0-1

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
