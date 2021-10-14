#include <string>
#include <vector> 
#include <string>
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
    v3& normalize(void);

    std::string toString() const;
};
#endif

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

    std::string toString() const;
    bool checkIfInside(v2*) const;
};
#endif

#ifndef objectClass
#define objectClass
class object {
  private:
    v3 internalCurTrans;
    float internalCurScalar;
  public:
    object():nv(0),np(0) {internalCurTrans = v3(0,0,0); internalCurScalar = 1; vertexes = nullptr; planes = nullptr;}

    object(unsigned int v, unsigned int p): nv(v), np(p) {
      vertexes = new v3[v];
      planes = new int[p*3];
      internalCurTrans = v3(0,0,0);
      internalCurScalar = 1;
    }

    object(std::vector<v3> ver, std::vector<int> pla): nv{ver.size()}, np{pla.size()/3} {
      vertexes = new v3[nv];
      planes = new int[np*3];
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
    }

    object& operator=(const object& other) {
      nv = other.nv;
      np = other.np;
      vertexes = other.vertexes;
      planes = other.planes;
      internalCurTrans = other.internalCurTrans;
      internalCurScalar = other.internalCurScalar;
      return *this;
    }

    ~object(void) {
      if (nv == 0 && np == 0) return;
      delete vertexes;
      delete planes;
    }

    std::string toString();

    long unsigned int nv;
    long unsigned int np;
    v3* vertexes;
    int* planes; //!! must be all be 3 elements (triangles), will only consider first 3
    //idx%3 are the elements of that plane, idx/3 is the index of the plane itself


    void rotateX(float);
    void rotateY(float);
    void rotateZ(float);
    void translate(v3);
    const v3& currentTranslation = internalCurTrans;
    /*
     * scales around the origin, if you want self scaling do 
     * v3 hold = currentTranslation;
     * translate(currentTranslation * -1);
     * scale(scalar);
     * translate(hold);
     */
    void scale(float);
    const float& currentScalar = internalCurScalar;
};
object objectFromFile(std::string);
#endif

//bool checkIfInside(v2, v2*, unsigned int);
