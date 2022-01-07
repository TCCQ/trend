#include "matrix.hh"
#include <string>
#include <iostream>
#include <math.h>


m33::m33() {
  elements = new float[9];
}

m33::m33(const m33& other) {
  elements = new float[9];
  for (int i = 0; i< 9; i++) {
    this->elements[i] = other.elements[i];
  }
}

m33& m33::operator=(const m33& other) {
  for (int i = 0; i< 9; i++) {
    this->elements[i] = other.elements[i];
  }
  return *this;
}

m33::~m33() {
  delete [] elements;
}

float m33::get(int r, int c) const {
  return elements[r*3 + c];
}

void m33::set(int r, int c, float v) {
  elements[r*3 + c] = v;
}

v3 m33::getCol(int c) const {
  return v3(get(0,c), get(1,c), get(2,c));
}

v3 m33::getRow(int r) const {
  return v3(get(r,0), get(r,1), get(r,2));
}

void m33::setCol(int c, v3 v) {
  set(0,c,v.x);
  set(1,c,v.y);
  set(2,c,v.z);
}

void m33::setRow(int r, v3 v) {
  set(r,0,v.x);
  set(r,1,v.y);
  set(r,2,v.z);
}

float m33::getLeadingEntry(int r) const {
  for (int i =0 ; i < 3; i++) {
    float cur = get(r,i);
    if (cur != 0) return cur;
  }
  return 0;
}

void m33::swapRows(int a, int b) {
  float first[3] = {get(a,0), get(a,1), get(a,2)};
  for (int c = 0; c < 3; c++) {
    set(a,c, get(b,c));
    set(b,c, first[c]);
  }
}

void m33::multiplyRow(int r, float m) {
  for (int i = 0; i < 3; i++) {
    set(r,i, get(r,i) * m);
  }
}

void m33::addMultRow(int a, int b, float m) {
  for (int i = 0; i < 3; i++) {
    set(b,i, get(b,i) + get(a,i) * m);
  }
}

//just here for posterity basically. You want to use getInverse or one of the multiplies probably
void m33::reduce() {
  int pivotRow;
  for (int r = 0; r < 3; r++) {
//    /*
    pivotRow = r;
    for (int i = r+1; i < 3; i++) {
      if (abs(get(i,r)) > abs(get(pivotRow,r))) pivotRow = i;
    } 
    swapRows(r,pivotRow); //pick biggest of the leading entries
    //above is optimization to reduce rounding errors
//    */
    
    float cur = get(r,r);
    int nextRow = r+1;
    while (cur == 0) {
      swapRows(r,nextRow);
      cur = get(r,r);
      nextRow++;
    }
    //TODO do singlular checks
    multiplyRow(r,1/cur); //get leading 1
//    std::cout << toString() << std::endl <<std::endl;

    for (int i = 0; i < 3; i++) {
      if (i == r) continue; //skip the row with the leading entry we are working with
      float toElim = get(i,r);
      addMultRow(r,i,-1*toElim); //should come out to zero
    }  
  }  
//  std::cout << toString() << std::endl <<std::endl;
//  std::cout <<std::endl <<std::endl;
}

m33 m33::getInverse(void) const {
  m33 original = m33(*this);
  m33 inverse;
  inverse.setCol(0,v3(1,0,0));
  inverse.setCol(1,v3(0,1,0));
  inverse.setCol(2,v3(0,0,1));

  int pivotRow;
  for (int r = 0; r < 3; r++) {
//    /*
    pivotRow = r;
    for (int i = r+1; i < 3; i++) {
      if (abs(original.get(i,r)) > abs(original.get(pivotRow,r))) pivotRow = i;
    } 
    original.swapRows(r,pivotRow); 
    inverse.swapRows(r,pivotRow); 
//    */

    float cur = original.get(r,r);
    int nextRow = r+1;
    while (cur == 0) {
      original.swapRows(r,nextRow);
      inverse.swapRows(r,nextRow);
      cur = original.get(r,r);
      nextRow++;
    }
    //TODO do singularity checks
    original.multiplyRow(r,1/cur); //get leading 1
    inverse.multiplyRow(r,1/cur); 

    for (int i = 0; i < 3; i++) {
      if (i == r) continue; 
      float toElim = original.get(i,r);
      original.addMultRow(r,i,-1*toElim); 
      inverse.addMultRow(r,i,-1*toElim); 
    }  
  }  
//  std::cout << this->toString() << std::endl << std::endl;
//  std::cout << inverse.toString() << std::endl << std::endl;
  return inverse;
}

v3 m33::multLeft(const v3 onRight) const {
  return v3(
        onRight.dot(getRow(0)),     
        onRight.dot(getRow(1)),     
        onRight.dot(getRow(2))     
      );
}

v3 m33::multRight(const v3 onLeft) const {
  return v3(
        onLeft.dot(getCol(0)),     
        onLeft.dot(getCol(1)),     
        onLeft.dot(getCol(2))     
      ); 
}

m33 m33::multLeft(const m33& onRight) const {
  m33 out;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      out.set(i,j, this->getRow(i).dot(onRight.getCol(j)));
    }
  }
  return out;
}

m33 m33::multRight(const m33& onRight) const {
  m33 out;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      out.set(i,j, this->getCol(j).dot(onRight.getRow(i)));
    }
  }
  return out;
}

std::string m33::toString() const {
  std::string out = "";
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      out += std::to_string(get(r,c)) + " ";
    }
    if (r != 2) out += "\n";
  }
  return out;
}

//END m33 methods

m44::m44() {
  elements = new float[16];
}

m44::m44(const m44& other) {
  elements = new float[16];
  for (int i = 0; i< 16; i++) {
    this->elements[i] = other.elements[i];
  }
}

m44& m44::operator=(const m44& other) {
  for (int i = 0; i< 16; i++) {
    this->elements[i] = other.elements[i];
  }
  return *this;
}

m44::~m44() {
  delete [] elements;
}

float m44::get(int r, int c) const {
  return elements[r*4 + c];
}

void m44::set(int r, int c, float v) {
  elements[r*4 + c] = v;
}

v4 m44::getCol(int c) const {
  return v4(get(0,c), get(1,c), get(2,c), get(3,c));
}

v4 m44::getRow(int r) const {
  return v4(get(r,0), get(r,1), get(r,2), get(r,3));
}

void m44::setCol(int c, v4 v) {
  set(0,c,v.x);
  set(1,c,v.y);
  set(2,c,v.z);
  set(3,c,v.w);
}

void m44::setRow(int r, v4 v) {
  set(r,0,v.x);
  set(r,1,v.y);
  set(r,2,v.z);
  set(r,3,v.w);
}

float m44::getLeadingEntry(int r) const {
  for (int i =0 ; i < 4; i++) {
    float cur = get(r,i);
    if (cur != 0) return cur;
  }
  return 0;
}

void m44::swapRows(int a, int b) {
  float first[4] = {get(a,0), get(a,1), get(a,2), get(a,3)};
  for (int c = 0; c < 4; c++) {
    set(a,c, get(b,c));
    set(b,c, first[c]);
  }
}

void m44::multiplyRow(int r, float m) {
  for (int i = 0; i < 4; i++) {
    set(r,i, get(r,i) * m);
  }
}

void m44::addMultRow(int a, int b, float m) {
  for (int i = 0; i < 4; i++) {
    set(b,i, get(b,i) + get(a,i) * m);
  }
}

//just here for posterity basically. You want to use getInverse or one of the multiplies probably
void m44::reduce() {
  int pivotRow;
  for (int r = 0; r < 4; r++) {
//    /*
    pivotRow = r;
    for (int i = r+1; i < 4; i++) {
      if (abs(get(i,r)) > abs(get(pivotRow,r))) pivotRow = i;
    } 
    swapRows(r,pivotRow); //pick biggest of the leading entries
    //above is optimization to reduce rounding errors
//    */
    
    float cur = get(r,r);
    int nextRow = r+1;
    while (cur == 0) {
      swapRows(r,nextRow);
      cur = get(r,r);
      nextRow++;
    }
    //TODO do singlular checks
    multiplyRow(r,1/cur); //get leading 1
//    std::cout << toString() << std::endl <<std::endl;

    for (int i = 0; i < 4; i++) {
      if (i == r) continue; //skip the row with the leading entry we are working with
      float toElim = get(i,r);
      addMultRow(r,i,-1*toElim); //should come out to zero
    }  
  }  
//  std::cout << toString() << std::endl <<std::endl;
//  std::cout <<std::endl <<std::endl;
}

m44 m44::getInverse(void) const {
  m44 original = m44(*this);
  m44 inverse;
  inverse.setCol(0,v4(1,0,0,0));
  inverse.setCol(1,v4(0,1,0,0));
  inverse.setCol(2,v4(0,0,1,0));
  inverse.setCol(3,v4(0,0,0,1));

  int pivotRow;
  for (int r = 0; r < 4; r++) {
//    /*
    pivotRow = r;
    for (int i = r+1; i < r; i++) {
      if (abs(original.get(i,r)) > abs(original.get(pivotRow,r))) pivotRow = i;
    } 
    original.swapRows(r,pivotRow); 
    inverse.swapRows(r,pivotRow); 
//    */

    float cur = original.get(r,r);
    int nextRow = r+1;
    while (cur == 0) {
      original.swapRows(r,nextRow);
      inverse.swapRows(r,nextRow);
      cur = original.get(r,r);
      nextRow++;
    }
    //TODO do singularity checks
    original.multiplyRow(r,1/cur); //get leading 1
    inverse.multiplyRow(r,1/cur); 

    for (int i = 0; i < r; i++) {
      if (i == r) continue; 
      float toElim = original.get(i,r);
      original.addMultRow(r,i,-1*toElim); 
      inverse.addMultRow(r,i,-1*toElim); 
    }  
  }  
//  std::cout << this->toString() << std::endl << std::endl;
//  std::cout << inverse.toString() << std::endl << std::endl;
  return inverse;
}

v4 m44::multLeft(const v4 onRight) const {
  return v4(
        onRight.dot(getRow(0)),     
        onRight.dot(getRow(1)),     
        onRight.dot(getRow(2)),     
        onRight.dot(getRow(3))
      );
}

v4 m44::multRight(const v4 onLeft) const {
  return v4(
        onLeft.dot(getCol(0)),     
        onLeft.dot(getCol(1)),     
        onLeft.dot(getCol(2)),
        onLeft.dot(getCol(3))
      ); 
}

m44 m44::multLeft(const m44& onRight) const {
  m44 out;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      out.set(i,j, this->getRow(i).dot(onRight.getCol(j)));
    }
  }
  return out;
}

m44 m44::multRight(const m44& onRight) const {
  m44 out;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      out.set(i,j, this->getCol(j).dot(onRight.getRow(i)));
    }
  }
  return out;
}

std::string m44::toString() const {
  std::string out = "";
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      out += std::to_string(get(r,c)) + " ";
    }
    if (r != 3) out += "\n";
  }
  return out;
}
