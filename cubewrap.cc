#include "cubewrap.hh"
#include <math.h>
#include <string>

void cubeWrapper::insert(char a) {
  switch(a) {
    case 'F':
    case 'f':
    case 'B':
    case 'b':
    case 'R':
    case 'r':
    case 'L':
    case 'l':
    case 'T':
    case 't':
    case 'U':
    case 'u':
      buffer.push_back(a);
      break;
    default:
      break;
  }
}

void cubeWrapper::cycle(char a[4], char b[4], char c[4]) {
  char hold = sub[a[0]][b[0]][c[0]];
  sub[a[0]][b[0]][c[0]] = sub[a[1]][b[1]][c[1]];
  sub[a[1]][b[1]][c[1]] = sub[a[2]][b[2]][c[2]];
  sub[a[2]][b[2]][c[2]] = sub[a[3]][b[3]][c[3]];
  sub[a[3]][b[3]][c[3]] = hold;
//  char hold = sub[a[3]][b[3]][c[3]];
//  sub[a[3]][b[3]][c[3]] = sub[a[2]][b[2]][c[2]];
//  sub[a[2]][b[2]][c[2]] = sub[a[1]][b[1]][c[1]];
//  sub[a[1]][b[1]][c[1]] = sub[a[0]][b[0]][c[0]];
//  sub[a[0]][b[0]][c[0]] = hold;
}

void cubeWrapper::updateSub(char in) {
  //do corners the edges
  if (in == 'F') {
    char a[4] = {0,0,0,0};
    char b[4] = {0,2,2,0};
    char c[4] = {0,0,2,2};
    cycle(a,b,c);
    char d[4] = {0,0,0,0};
    char e[4] = {1,2,1,0};
    char f[4] = {0,1,2,1};
    cycle(d,e,f);
  } else if (in == 'f') {
    char a[4] = {0,0,0,0};
    char b[4] = {0,0,2,2};
    char c[4] = {0,2,2,0};
    cycle(a,b,c);
    char d[4] = {0,0,0,0};
    char e[4] = {0,1,2,1};
    char f[4] = {1,2,1,0};
    cycle(d,e,f);
  } else if (in == 'B') {
    char a[4] = {2,2,2,2};
    char b[4] = {0,0,2,2};
    char c[4] = {0,2,2,0};
    cycle(a,b,c);
    char d[4] = {2,2,2,2};
    char e[4] = {0,1,2,1};
    char f[4] = {1,2,1,0};
    cycle(d,e,f);
  } else if (in == 'b') {
    char a[4] = {2,2,2,2};
    char b[4] = {0,2,2,0};
    char c[4] = {0,0,2,2};
    cycle(a,b,c);
    char d[4] = {2,2,2,2};
    char e[4] = {1,2,1,0};
    char f[4] = {0,1,2,1};
    cycle(d,e,f);
  } else if (in == 'R') {
    char a[4] = {0,0,2,2};
    char b[4] = {0,2,2,0};
    char c[4] = {2,2,2,2};
    cycle(a,b,c);
    char d[4] = {0,1,2,1};
    char e[4] = {1,2,1,0};
    char f[4] = {2,2,2,2};
    cycle(d,e,f);
  } else if (in == 'r') {
    char a[4] = {0,2,2,0};
    char b[4] = {0,0,2,2};
    char c[4] = {2,2,2,2};
    cycle(a,b,c);
    char d[4] = {1,2,1,0};
    char e[4] = {0,1,2,1};
    char f[4] = {2,2,2,2};
    cycle(d,e,f);
  } else if (in == 'L') {
    char a[4] = {0,2,2,0};
    char b[4] = {0,0,2,2};
    char c[4] = {0,0,0,0};
    cycle(a,b,c);
    char d[4] = {1,2,1,0};
    char e[4] = {0,1,2,1};
    char f[4] = {0,0,0,0};
    cycle(d,e,f);
  } else if (in == 'l') {
    char a[4] = {0,0,2,2};
    char b[4] = {0,2,2,0};
    char c[4] = {0,0,0,0};
    cycle(a,b,c);
    char d[4] = {0,1,2,1};
    char e[4] = {1,2,1,0};
    char f[4] = {0,0,0,0};
    cycle(d,e,f);
  } else if (in == 'T') {
    char a[4] = {0,2,2,0};
    char b[4] = {2,2,2,2};
    char c[4] = {0,0,2,2};
    cycle(a,b,c);
    char d[4] = {1,2,1,0};
    char e[4] = {2,2,2,2};
    char f[4] = {0,1,2,1};
    cycle(d,e,f);
  } else if (in == 't') {
    char a[4] = {0,0,2,2};
    char b[4] = {2,2,2,2};
    char c[4] = {0,2,2,0};
    cycle(a,b,c);
    char d[4] = {0,1,2,1};
    char e[4] = {2,2,2,2};
    char f[4] = {1,2,1,0};
    cycle(d,e,f);
  } else if (in == 'U') {
    char a[4] = {0,0,2,2};
    char b[4] = {0,0,0,0};
    char c[4] = {0,2,2,0};
    cycle(a,b,c);
    char d[4] = {0,1,2,1};
    char e[4] = {0,0,0,0};
    char f[4] = {1,2,1,0};
    cycle(d,e,f);
  } else if (in == 'u') {
    char a[4] = {0,2,2,0};
    char b[4] = {0,0,0,0};
    char c[4] = {0,0,2,2};
    cycle(a,b,c);
    char d[4] = {1,2,1,0};
    char e[4] = {0,0,0,0};
    char f[4] = {0,1,2,1};
    cycle(d,e,f);
  }
}

std::string cubeWrapper::toString() {
  std::string out = "";
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      for (int z = 0; z < 3; z++) {
        out += std::to_string(sub[x][y][z]) + ",";
      }
      out += '\n';
    }
    out += '\n';
  }
  return out;
}

void cubeWrapper::tick() {
  if (midturn == 0 && buffer.empty()) return;
  if (midturn == 0) {
    updateSub(current);
    current = buffer.front();
    buffer.pop_front();
    midturn = numSteps;
  }
  if (current == 'F') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[0][i][j]].rotateX((M_PI/-2) / numSteps);
      }
    }
  } else if (current == 'f') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[0][i][j]].rotateX((M_PI/2) / numSteps);
      }
    }
  } else if (current == 'B') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[2][i][j]].rotateX((M_PI/2) / numSteps);
      }
    }
  } else if (current == 'b') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[2][i][j]].rotateX((M_PI/-2) / numSteps);
      }
    }
  } else if (current == 'R') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[i][j][2]].rotateZ((M_PI/2) / numSteps);
      }
    }
  } else if (current == 'r') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[i][j][2]].rotateZ((M_PI/-2) / numSteps);
      }
    }
  } else if (current == 'L') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[i][j][0]].rotateZ((M_PI/-2) / numSteps);
      }
    }
  } else if (current == 'l') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[i][j][0]].rotateZ((M_PI/2) / numSteps);
      }
    }
  } else if (current == 'T') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[i][2][j]].rotateY((M_PI/2) / numSteps);
      }
    }
  } else if (current == 't') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[i][2][j]].rotateY((M_PI/-2) / numSteps);
      }
    }
  } else if (current == 'U') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[i][0][j]].rotateY((M_PI/-2) / numSteps);
      }
    }
  } else if (current == 'u') {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        visible[sub[i][0][j]].rotateY((M_PI/2) / numSteps);
      }
    }
  }
  midturn--;
}
