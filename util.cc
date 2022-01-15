#include "util.hh"
#include <iostream>
#include <math.h> //for sqrt and pow
#include <vector>
#include <fstream>


//v3 methods

std::string v3::toString() const {
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"; 
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

//END v3 methods

//v2 methods

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

//END v2 methods

//object methods

object object::fromFile(std::string str) {
  std::fstream file = std::fstream(str, std::ifstream::in);
  if (!file.is_open()) return object();

  std::vector<v3>ver = std::vector<v3>();
  std::vector<int>pla = std::vector<int>();
  std::vector<v2>textureLocs = std::vector<v2>();
  std::vector<int>textureIdx = std::vector<int>();
  std::string word;
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

//v4 methods

std::string v4::toString() const {
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")"; 
}

//END v4 methods
