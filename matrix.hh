#include "util.hh"
#include <string>

#ifndef MATRIXCLASS
#define MATRIXCLASS
class m33 {
  private:
    float* elements;
  public:
    m33();
    m33(const m33&);
    ~m33();
    m33& operator=(const m33&);

    v3 multLeft(const v3) const;
    v3 multRight(const v3) const;
    m33 multLeft(const m33&) const;
    m33 multRight(const m33&) const;


    float get(int,int) const;
    void set(int,int,float);
    v3 getCol(int) const;
    v3 getRow(int) const;
    void setCol(int,v3);
    void setRow(int,v3);

    float getLeadingEntry(int) const;
  
    void multiplyRow(int,float);
    void addMultRow(int,int,float);
    void swapRows(int,int);

    void reduce(void);
    m33 getInverse(void) const;

    std::string toString(void) const;
};
#endif
