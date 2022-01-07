#include "util.hh"
#include <iostream>
#include <math.h> //for sqrt and pow
#include <string>
#include <vector>
#include <fstream>

float v3::length() const {
  return sqrt(x*x + y*y + z*z);
}

//same as (*this - other).length()
float v3::distance(const v3& other) const {
  return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2) + pow(z - other.z, 2));
}

float v3::dot(const v3& other) const {
  return x*other.x + y*other.y + z*other.z;
}

v3 v3::cross(const v3& other) const {
  return v3(y*other.z - z*other.y, 
            z*other.x - x*other.z,
            x*other.y - y*other.x);
}

v3 v3::operator+(const v3& other) const {
  return v3(x+other.x, y+other.y, z+other.z);
}

v3 v3::operator-(const v3& other) const {
  return v3(x-other.x, y-other.y, z-other.z);
}

v3 v3::operator*(const float scalar) const {
  return v3(x*scalar, y*scalar, z*scalar);
}

v3& v3::operator+=(const v3& other) {
  x += other.x;
  y += other.y;
  z += other.z;
  return *this;
}

v3& v3::operator-=(const v3& other) {
  x -= other.x;
  y -= other.y;
  z -= other.z;
  return *this;
} 

v3& v3::operator*=(const float scalar) {
  x *= scalar;
  y *= scalar;
  z *= scalar;
  return *this;
}

v3& v3::operator=(const v3& other) {
  x = other.x;
  y = other.y;
  z = other.z;
  return *this;
}

std::string v3::toString() const {
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"; 
}

v3& v3::normalize(void) {
  float len = length();
  x /= len;
  y /= len;
  z /= len;
  return *this;
}

v3& v3::rotX(float theta) {
  float ny = cos(theta)*y - sin(theta)*z;
  float nz = sin(theta)*y + cos(theta)*z;
  y = ny;
  z = nz;
  return *this;
}

v3& v3::rotY(float theta) {
  float nz = cos(theta)*z - sin(theta)*x;
  float nx = sin(theta)*z + cos(theta)*x;
  z = nz;
  x = nx;
  return *this;
}

v3& v3::rotZ(float theta) {
  float nx = cos(theta)*x - sin(theta)*y;
  float ny = sin(theta)*x + cos(theta)*y;
  x = nx;
  y = ny;
  return *this;
}

//END OF v3 methods

v2 v2::operator+(const v2& other) const {
  return v2(x+other.x, y+other.y);
}

v2 v2::operator-(const v2& other) const {
  return v2(x-other.x, y-other.y);
}

v2 v2::operator*(const float& other) const {
  return v2(x*other, y*other);
}

v2& v2::operator+=(const v2& other) {
  x += other.x;
  y += other.y;
  return *this;
}

v2& v2::operator-=(const v2& other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

v2& v2::operator*=(const float& other) {
  x *= other;
  y *= other;
  return *this;
}

v2& v2::operator=(const v2& other) {
  x = other.x;
  y = other.y;
  return *this;
}

float v2::dot(const v2& other) const {
  return other.x*x + other.y*y;
}

v2 v2::rayNormal(void) const {
  return v2(-y,x);
}

std::string v2::toString() const {
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

//pointer to 3 long array of v2s, must be in counter-clockwise order
bool v2::checkIfInside(v2* bound) const {
  float f,s,t;
  v2 rela = (*this)-bound[0];
  v2 ab = bound[1] - bound[0];
  f = rela.dot(ab.rayNormal());

  v2 relb = (*this)-bound[1];
  v2 bc = bound[2] - bound[1];
  s = relb.dot(bc.rayNormal());

  v2 relc = (*this)-bound[2];
  v2 ca = bound[0] - bound[2];
  t = relc.dot(ca.rayNormal());

  if (f >= 0 && s >= 0 && t >= 0) return true;
  else if (f <= 0 && s <= 0 && t <= 0) return true;
  else return false;
}

/*
#define max(a,b) ((a>b)? a:b)
#define min(a,b) ((a<b)? a:b)
//move right and count crossings
//do not include duplicate vertex at the end, not necessary
bool checkIfInside(v2 p, v2* boundary, unsigned int size) {
  if (size < 3) return false;
  v2 a,b;
  a = boundary[0];
  unsigned int crossings = 0;
  for (unsigned long int idx = 1; idx <= size; idx++) {
    b = boundary[idx % size];
    if (p.x <= a.x || p.x <= b.x) { //to the left of at least one end, or on one
      if (p.y > min(a.y,b.y) && p.y <= max(a.y,b.y)) { //between them vertically
        //NOTE!!! that it can be equal to the top, but not the bottom,
        if (a.y != b.y) { //do not allow horizontal lines
          //get x value of the line between a and b at the y value of p
          float tx = ((b.x - a.x)/(b.y - a.y))*(p.y - a.y) + a.x;
          std::cerr << "(" << p.x << "," << p.y << ") : " << tx <<std::endl;
          if (p.x <= tx) {
            std::cerr << "passed" << std::endl;
            ++crossings;
          }
        }
      }
    }
    a = b;
  }
  if (crossings % 2 == 1) {
    //odd
    return true;
  } else return false;
}
*/

//END v2 methods

object object::fromFile(std::string str) {
  std::fstream file = std::fstream(str, std::ifstream::in);
  if (!file.is_open()) return object();

  std::vector<v3>ver = std::vector<v3>();
  std::vector<int>pla = std::vector<int>();
  std::vector<v2>textureLocs = std::vector<v2>();
  std::vector<int>textureIdx = std::vector<int>();
  std::string word;
  int st,end;
  float perLine[3];
  do {
    file >> str;
    if (str == "v") {
      for (int i =0; i < 3; i++) {
        file >> str;
//        str = str.substr(0, str.find('/',0)); //get only first value
        perLine[i] = std::atof(str.c_str());
      }  
      ver.push_back(v3(perLine[0],perLine[1],perLine[2]));
    } else if (str == "f") {
      for (int i =0; i < 3; i++) {
        file >> str;
        int fSlash = str.find('/',0);
        //isolate first value
        pla.push_back(std::atoi(str.substr(0,fSlash).c_str())-1);
        if (fSlash != std::string::npos) {
          //there are texture idx
          str = str.substr(fSlash+1,str.find('/', fSlash+1));
          textureIdx.push_back(std::atoi(str.c_str())-1);
        }
      }  
    } else if (str == "vt") {
      for (int i =0; i < 2; i++) {
        file >> str;
        float value = std::atof(str.c_str());
        value = abs(value);
        if (i==1) value = 1 - value;
        perLine[i] = value;
      }  
      textureLocs.push_back(v2(perLine[0], perLine[1]));
    }
  } while (!(file.eof()));
  file.close();
  return object(ver,pla,textureLocs,textureIdx);
}

std::string object::toString() {
  std::string out = "";
  for (int v =0; v < nv; v++) {
    out += vertexes[v].toString() + " ";
  }
  out += '\n';
  for (int p=0; p < np; p++) {
    out += "[" + std::to_string(planes[p*3]) +","+std::to_string(planes[(p*3)+1])+","+std::to_string(planes[(p*3)+2])+"] "; 
  }
  return out;
}

void object::translate(v3 trans) {
  for (int i = 0; i < nv; i++) {
    vertexes[i] += trans;
  }
  internalCurTrans += trans;
}

void object::scale(float scalar) {
  v3 hold = currentTranslation;
  translate(hold * -1);
  for (int i = 0; i< nv; i++) {
    vertexes[i] *= scalar;
  }
  internalCurScalar *= scalar;
  translate(hold);
}

void object::rotateX(float theta) {
//  v3 hold = currentTranslation;
//  translate(hold * -1);
//  v3 tmp;
  for (int i = 0; i < nv; i++) {
    vertexes[i].rotX(theta);
//    v3& cur = vertexes[i];
//    tmp = v3(cur.x, cur.y * cos(theta) + cur.z * sin(theta), cur.y * -1 * sin(theta) + cur.z * cos(theta));
//    vertexes[i] = tmp;
  }
//  translate(hold);
}

void object::rotateY(float theta) {
//  v3 hold = currentTranslation;
//  translate(hold * -1);
//  v3 tmp;
  for (int i = 0; i < nv; i++) {
    vertexes[i].rotY(theta);
//    v3& cur = vertexes[i];
//    tmp = v3(cur.x * cos(theta) + cur.z * sin(theta), cur.y, cur.x * -1 * sin(theta) + cur.z * cos(theta));
//    vertexes[i] = tmp;
  }
//  translate(hold);
}

void object::rotateZ(float theta) {
//  v3 hold = currentTranslation;
//  translate(hold * -1);
//  v3 tmp;
  for (int i = 0; i < nv; i++) {
    vertexes[i].rotZ(theta);
//    v3& cur = vertexes[i];
//    tmp = v3(cur.x * cos(theta) + cur.y * sin(theta), cur.x * -1 * sin(theta) + cur.y * cos(theta), cur.z);
//    vertexes[i] = tmp;
  }
//  translate(hold);
}

object object::objAndTexture(std::string oFile, std::string tFile) {
  object out = object::fromFile(oFile);
  out.texture = timage::fromFile(tFile);
  return out;
}

//END object methods

float v4::length(void) const {
  return sqrt(x*x + y*y + z*z + w*w);
}

float v4::distance(const v4& other) const {
  return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2) + pow(z - other.z, 2) + pow(w - other.w, 2));
}

float v4::dot(const v4& other) const {
  return other.x*x + other.y*y + other.z*z + other.w*w;
}

v4 v4::operator+(const v4& other) const {
  return v4(x+other.x, y+other.y, z+other.z, w+other.w);
}

v4 v4::operator-(const v4& other) const {
  return v4(x-other.x, y-other.y, z-other.z, w-other.w);
}

v4 v4::operator*(const float scalar) const {
  return v4(x*scalar, y*scalar, z*scalar, w*scalar);
}

v4& v4::operator+=(const v4& other) {
  x += other.x;
  y += other.y;
  z += other.z;
  w += other.w;
  return *this;
}

v4& v4::operator-=(const v4& other) {
  x -= other.x;
  y -= other.y;
  z -= other.z;
  w -= other.w;
  return *this;
}

v4& v4::operator*=(const float scalar) {
  x *= scalar;
  y *= scalar;
  z *= scalar;
  w *= scalar;
  return *this;
}

v4& v4::operator=(const v4& other) {
  x = other.x;
  y = other.y;
  z = other.z;
  w = other.w;
  return *this;
}

v4& v4::normalize(void) {
  return *this *= length();
}

std::string v4::toString() const {
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")"; 
}

