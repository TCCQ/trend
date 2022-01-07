#include <string>
#include <vector> 
#include <string>
#include "timage.hh"
#include <iostream>
//the dynamic array vector, not a math vector

#ifndef v2Class
#define v2Class
class v2 {
  public:
    float x,y;
    v2(void) {}
    v2(float i, float j): x(i), y(j) {}
    v2 rayNormal(void) const;
    float dot(const v2&) const;
    v2 operator+(const v2&) const;
    v2 operator-(const v2&) const;
    v2 operator*(const float&) const;
    v2& operator+=(const v2&);
    v2& operator-=(const v2&);
    v2& operator*=(const float&);
    v2& operator=(const v2&);

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
    v3(v2 in): x(in.x), y(in.y), z(0) {}

    float length(void) const; 
    float distance(const v3&) const;
    float dot(const v3&) const;
    v3 cross(const v3&) const;
    v3 operator+(const v3&) const;
    v3 operator-(const v3&) const;
    v3 operator*(const float) const;    
    v3& operator+=(const v3&);
    v3& operator-=(const v3&);
    v3& operator*=(const float);    
    v3& operator=(const v3&);
    v3& normalize(void);
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
    v4(v3 in): x(in.x), y(in.y), z(in.z), w(1) {}

    float length(void) const; 
    float distance(const v4&) const;
    float dot(const v4&) const;
    v4 operator+(const v4&) const;
    v4 operator-(const v4&) const;
    v4 operator*(const float) const;    
    v4& operator+=(const v4&);
    v4& operator-=(const v4&);
    v4& operator*=(const float);    
    v4& operator=(const v4&);
    v4& normalize(void);

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

    ~object(void) {
      if (nv == 0 && np == 0) return;
//      delete [] vertexes;
//      delete [] planes;
//      delete [] UVs;
//      delete [] planeUVs;
    }

    static object fromFile(std::string);
    static object objAndTexture(std::string, std::string);

    std::string toString();

    long unsigned int nv;
    long unsigned int np;
    std::vector<v3> vertexes;
    std::vector<int> planes; //!! must be all be 3 elements (triangles), will only consider first 3
    //idx%3 are the elements of that plane, idx/3 is the index of the plane itself
    std::vector<v2> UVs;
    std::vector<int> planeUVs;
    timage texture;


    void rotateX(float);
    void rotateY(float);
    void rotateZ(float);
    void translate(v3);
    const v3& currentTranslation = internalCurTrans;
    void scale(float);
    const float& currentScalar = internalCurScalar;
};
#endif

//bool checkIfInside(v2, v2*, unsigned int);
