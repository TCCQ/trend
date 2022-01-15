#include <string>
#include <vector> 
//the dynamic array vector, not a math vector
#include <math.h> //for sqrt and pow
#include <string>
#include "timage.hh"
#include <iostream>

//make the basic operators inline
//requres the func to be declared and defined in the same file 
//(as far as I can tell)
#define SMALL inline __attribute__((always_inline))

/*
 * so for some reason gcc warns about inplicitly defined constructors
 * specifically type(const type&) 
 * which I didn't have, cause I had a defined operator= which did the same thing
 * but apparently it would invisibly create a constructor that did that from the operator=
 * instead of just use the void constructor and then use the operator (which is what I expected
 * so I explicitly defined those too
 */

#ifndef v2Class
#define v2Class
class v2 {
  public:
    float x,y;
    v2(void) {}
    v2(float i, float j): x(i), y(j) {}
    v2(const v2& other): x(other.x), y(other.y) {}

    //declared AND defined here, so that it can be inlined see SMALL def
    SMALL float dot(const v2& other) const { return other.x*x + other.y*y; }
    SMALL float length(void) const { return sqrt(x*x + y*y); }
    SMALL v2 operator+(const v2& other) const { return v2(x+other.x, y+other.y); }
    SMALL v2 operator-(const v2& other) const { return v2(x-other.x, y-other.y); }
    SMALL v2 operator*(const float& other) const { return v2(x*other, y*other); }
    SMALL v2& operator+=(const v2& other) { x += other.x; y += other.y; return *this; }
    SMALL v2& operator-=(const v2& other) { x -= other.x; y -= other.y; return *this; }
    SMALL v2& operator*=(const float& other) { x *= other; y *= other; return *this; }
    SMALL v2& operator=(const v2& other) { x = other.x; y = other.y; return *this; }
    SMALL v2& normalize(void) { float len = length(); x /= len; y /= len; return *this; }

    v2 rayNormal(void) const;
    std::string toString() const;
    bool checkIfInside(v2*) const;
};
#endif

#ifndef v3Class
#define v3Class
class v3 {
  public:
    float x,y,z;

    v3(void) {}
    v3(float i, float j, float k): x(i), y(j), z(k) {} 
    v3(const v2& in): x(in.x), y(in.y), z(0) {}
    v3(const v3& other): x(other.x), y(other.y), z(other.z) {}

    //declared AND defined here, so that it can be inlined see SMALL def
    SMALL float length(void) const { return sqrt(x*x + y*y + z*z); }
    SMALL float distance(const v3& other) const { return (*this - other).length(); }
    SMALL float dot(const v3& other) const { return x*other.x + y*other.y + z*other.z; }
    SMALL v3 cross(const v3& other) const { 
       return v3(y*other.z - z*other.y,
                 z*other.x - x*other.z,
                 x*other.y - y*other.x);
    }
    SMALL v3 operator+(const v3& other) const { return v3(x+other.x, y+other.y, z+other.z); }
    SMALL v3 operator-(const v3& other) const { return v3(x-other.x, y-other.y, z-other.z); }
    SMALL v3 operator*(const float scalar) const { return v3(x*scalar, y*scalar, z*scalar); }
    SMALL v3& operator+=(const v3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    SMALL v3& operator-=(const v3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; } 
    SMALL v3& operator*=(const float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    SMALL v3& operator=(const v3& other) { x = other.x; y = other.y; z = other.z; return *this; }
    SMALL v3& normalize(void) { float len = length(); x /= len; y /= len; z /= len; return *this; }

    v3& rotX(float);
    v3& rotY(float);
    v3& rotZ(float);

    std::string toString() const;
};
#endif

#ifndef v4Class
#define v4Class
class v4 {
  public:
    float x,y,z,w;

    v4(void) {}
    v4(float i, float j, float k, float l): x(i), y(j), z(k), w(l) {} 
    //remember that it sets the fourth coord to 1 for use in unified transforms
    v4(const v3& in): x(in.x), y(in.y), z(in.z), w(1) {}
    v4(const v4& other): x(other.x), y(other.y), z(other.z), w(other.w) {}

    //declared AND defined here, so that it can be inlined see SMALL def
    SMALL float length(void) const { return sqrt(x*x + y*y + z*z + w*w); }
    SMALL float distance(const v4& other) const { return (*this - other).length(); }
    SMALL float dot(const v4& other) const { return other.x*x + other.y*y + other.z*z + other.w*w; }
    SMALL v4 operator+(const v4& other) const { return v4(x+other.x, y+other.y, z+other.z, w+other.w); }
    SMALL v4 operator-(const v4& other) const { return v4(x-other.x, y-other.y, z-other.z, w-other.w); }
    SMALL v4 operator*(const float scalar) const { return v4(x*scalar, y*scalar, z*scalar, w*scalar); }
    SMALL v4& operator+=(const v4& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
    SMALL v4& operator-=(const v4& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
    SMALL v4& operator*=(const float scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
    SMALL v4& operator=(const v4& other) { x = other.x; y = other.y; z = other.z; w = other.w; return *this; }
    SMALL v4& normalize(void) { return *this *= length(); }

    std::string toString() const;
};
#endif

#ifndef objectClass
#define objectClass
class object {
  private:
    v3 internalCurTrans;
    float internalCurScalar;
  public:
    //const references (read-only)
    //gives visiblity but forces user to use the methods instead of setting directly
    //important because just setting the variable does nothing, its just for reference
    const v3& currentTranslation = internalCurTrans; 
    const float& currentScalar = internalCurScalar;

    long unsigned int nv;
    long unsigned int np;
    std::vector<v3> vertexes;
    std::vector<int> planes; //!! must be all be 3 elements (triangles)
    //idx%3 are the elements of that plane, idx/3 is the index of the plane itself
    std::vector<v2> UVs;
    std::vector<int> planeUVs; 
    timage texture;

    //start methods and constructors

    object():nv(0),np(0) {
      internalCurTrans = v3(0,0,0);
      internalCurScalar = 1;
//      vertexes = nullptr;
//      planes = nullptr;
//      UVs = nullptr;
//      planeUVs = nullptr;
    }

    object(unsigned int v, unsigned int p): nv(v), np(p) {
      texture = timage();
      internalCurTrans = v3(0,0,0);
      internalCurScalar = 1;
    }

    object(std::vector<v3> ver, std::vector<int> pla): nv{ver.size()}, np{pla.size()/3} {
      vertexes = std::vector<v3>(ver);
      planes = std::vector<int>(pla);
      for (int i = 0; i < nv; i++) {
        vertexes[i] = ver[i];
      }
      int tmp;
      for (int i = 0; i < np; i++) {
        tmp = i*3;
        planes[tmp] = pla[tmp];
        planes[tmp+1] = pla[tmp+1];
        planes[tmp+2] = pla[tmp+2];
      }
      internalCurTrans = v3(0,0,0);
      internalCurScalar = 1;
      texture = timage();
    }

    object(std::vector<v3> ver, std::vector<int> pla, std::vector<v2> u, std::vector<int> uvp): nv{ver.size()}, np{pla.size()/3} {
      vertexes = std::vector<v3>(ver);
      planes = std::vector<int>(pla);
      UVs = std::vector<v2>(u);
      planeUVs = std::vector<int>(uvp);
      for (int i = 0; i < nv; i++) {
        vertexes[i] = ver[i];
      }
      for (int j = 0; j < u.size(); j++) {
        UVs[j] = u[j];
      }
      int tmp;
      for (int i = 0; i < np; i++) {
        tmp = i*3;
        planes[tmp] = pla[tmp];
        planes[tmp+1] = pla[tmp+1];
        planes[tmp+2] = pla[tmp+2];
        if (uvp.size() > 0) {
          planeUVs[tmp] = uvp[tmp];
          planeUVs[tmp+1] = uvp[tmp+1];
          planeUVs[tmp+2] = uvp[tmp+2];
        }
       }
      internalCurTrans = v3(0,0,0);
      internalCurScalar = 1;
    }

    object(const object& other) {
      nv = other.nv;
      np = other.np;
      vertexes = std::vector<v3>(other.vertexes);
      planes = std::vector<int>(other.planes);
      UVs = std::vector<v2>(other.UVs);
      planeUVs = std::vector<int>(other.planeUVs);

      texture = timage(other.texture);
      internalCurTrans = other.internalCurTrans;
      internalCurScalar = other.internalCurScalar;
    }

    object& operator=(const object& other) {
      nv = other.nv;
      np = other.np;
      vertexes = std::vector<v3>(other.vertexes);
      planes = std::vector<int>(other.planes);
      UVs = std::vector<v2>(other.UVs);
      planeUVs = std::vector<int>(other.planeUVs);

      texture = timage(other.texture);
      internalCurTrans = other.internalCurTrans;
      internalCurScalar = other.internalCurScalar;
      return *this;
    }

    static object fromFile(std::string);
    static object objAndTexture(std::string, std::string);

    std::string toString();

    void rotateX(float);
    void rotateY(float);
    void rotateZ(float);
    void translate(v3);
    void scale(float);
};
#endif
