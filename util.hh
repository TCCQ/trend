#include <string>
#include <vector> 
//the dynamic array vector, not a math vector

#ifndef v3Class
#define v3Class
class v3 {
  public:
    float x,y,z;

    v3(void) {}
    v3(float i, float j, float k): x(i), y(j), z(k) {} 

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

    std::string toString() const;
};
#endif

#ifndef pntClass
#define pntClass
class pnt {
  public:
    float x,y;
    pnt(void) {}
    pnt(float i, float j): x(i), y(j) {}
    pnt operator+(const pnt&) const;
    pnt operator-(const pnt&) const;
    pnt operator*(const float&) const;
    pnt& operator+=(const pnt&);
    pnt& operator-=(const pnt&);
    pnt& operator*=(const float&);

    std::string toString() const;
};
#endif

#ifndef objectClass
#define objectClass
class object {
  public:
    object(unsigned int v, unsigned int p): nv(v), np(p) {
      vertexes = new v3[v];
      planes = new int[p*3];
      translation  = v3(0,0,0);
    }

    object(std::vector<v3> ver, std::vector<std::vector<int>> pla): nv{ver.size()}, np{pla.size()} {
      vertexes = new v3[nv];
      planes = new int[np*3];
      for (int i = 0; i < nv; i++) {
        vertexes[i] = ver[i];
      }
      int tmp;
      for (int i = 0; i < np; i++) {
        tmp = i*3;
        planes[tmp] = pla[i][0];
        planes[tmp+1] = pla[i][1];
        planes[tmp+2] = pla[i][2];
      }
    }

    ~object(void) {
      delete vertexes;
      delete planes;
    }

    const long unsigned int nv;
    const long unsigned int np;
    v3* vertexes;
    int* planes; //!! must be all be 3 elements (triangles), will only consider first 3
    //idx%3 are the elements of that plane, idx/3 is the index of the plane itself


    v3 translation;
    //collection of vertex
    //collection of pnts, one for each vertex
    //collection of planes, each is collection of indexies in vertex list
    //array size for above 
};
#endif

bool checkIfInside(pnt, pnt*, unsigned int);
