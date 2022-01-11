#include <cstdint>
#include <string>

#ifndef TCOLORCLASS
#define TCOLORCLASS
class tcolor {
  public:
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;

    tcolor();
    tcolor(uint32_t);
    tcolor(uint8_t,uint8_t,uint8_t);
    tcolor(uint8_t,uint8_t,uint8_t,uint8_t);

    tcolor operator*(const float) const; 
    tcolor& operator*=(const float);
    tcolor& operator=(const tcolor&);
};
#endif

#ifndef TIMAGECLASS
#define TIMAGECLASS
class timage {
  public:
    uint32_t width;
    uint32_t height;
    tcolor* pixels;

    ~timage();
    timage();
    timage(const timage&); //deep copy
    timage(uint32_t, uint32_t);

    tcolor get(int,int) const;
    void set(int,int, tcolor);

    timage& operator=(timage&&); //swap
    static timage fromFile(std::string);
};
#endif
