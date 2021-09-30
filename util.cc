#include "util.hh"
#include <iostream>
#include <math.h> //for sqrt and pow
#include <string>

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

pnt pnt::operator+(const pnt& other) const {
  return pnt(x+other.x, y+other.y);
}

pnt pnt::operator-(const pnt& other) const {
  return pnt(x-other.x, y-other.y);
}

pnt pnt::operator*(const float& other) const {
  return pnt(x*other, y*other);
}

pnt& pnt::operator+=(const pnt& other) {
  x += other.x;
  y += other.y;
  return *this;
}

pnt& pnt::operator-=(const pnt& other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

pnt& pnt::operator*=(const float& other) {
  x *= other;
  y *= other;
  return *this;
}

std::string pnt::toString() const {
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

std::string v3::toString() const {
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"; 
}

#define max(a,b) ((a>b)? a:b)
#define min(a,b) ((a<b)? a:b)
//move right and count crossings
//do not include duplicate vertex at the end, not necessary
bool checkIfInside(pnt p, pnt* boundary, unsigned int size) {
  if (size < 3) return false;
  pnt a,b;
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
//          std::cerr << "(" << p.x << "," << p.y << ") : " << tx <<std::endl;
          if (p.x <= tx) {
//            std::cerr << "passed" << std::endl;
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
