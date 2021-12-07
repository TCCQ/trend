#include <list>
#include "util.hh"
#include <string>

#ifndef CUBEWCLASS
#define CUBEWCLASS
class cubeWrapper {
  private:
    std::list<char> buffer = std::list<char>();
    const int numSteps = 10; //#frames per 90 degree turn
    int midturn = 0;
    char current = 0;

    void cycle(char a[4], char b[4], char c[4]);
  public:
    char sub[3][3][3];
    void updateSub(char);
    object visible[27];
    cubeWrapper() {
      char idx = 0;
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          for (int k = 0; k < 3; k++) {
            sub[k][j][i] = idx++;
          }
        }
      }
    }

    std::string toString();
    void insert(char);
    void tick();
};
#endif
