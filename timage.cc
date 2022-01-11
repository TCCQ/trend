#include "timage.hh"
#include <fstream>
#include <iostream>

tcolor::tcolor() {
  b = 0;
  g = 0;
  r = 0;
  a = 0;
}

tcolor::tcolor(uint32_t v) {
  b = v;
  g = v >> 8;
  r = v >> 16;
  a = v >> 24;
}

tcolor::tcolor(uint8_t red, uint8_t green, uint8_t blue) {
 r = red;
 b = blue;
 g = green;
 a = 0xFF;
}

tcolor::tcolor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) { 
  r = red;
  b = blue;
  g = green;
  a = alpha;
}

tcolor tcolor::operator*(const float s) const {
  return tcolor(
    (uint8_t)(r * s),
    (uint8_t)(g * s),
    (uint8_t)(b * s),
    a //should I scale alpha too? not planing on using it
  );
}

tcolor& tcolor::operator*=(const float s) {
  r = (uint8_t)(r * s);
  g = (uint8_t)(g * s);
  b = (uint8_t)(b * s);
  return *this;
}

tcolor& tcolor::operator=(const tcolor& other) {
  b = other.b;
  g = other.g;
  r = other.r;
  a = other.a;
  return *this;
}

timage::~timage() {
  delete [] pixels;
}

timage& timage::operator=(timage&& other) { //move assignment
  width = other.width;
  height = other.height;
  delete [] pixels;
  pixels = new tcolor[width * height];
  for (int i = 0; i < width * height; i++) {
    pixels[i] = other.pixels[i];
  }
  return *this;
}

timage::timage() {
  width = 0;
  height = 0;
  pixels = nullptr;
}

timage::timage(const timage& other) {
  width = other.width;
  height = other.height;
  pixels = new tcolor[width * height];
  for (int i = 0; i < width * height; i++) {
    pixels[i] = other.pixels[i];
  }
}

timage::timage(uint32_t w, uint32_t h) {
  width = w;
  height = h;
  if (w == 0 || h == 0) {
    pixels = nullptr;
  } else {
    pixels = new tcolor[w*h];
  }
}

timage timage::fromFile(std::string str) {
  std::fstream file = std::fstream(str, std::fstream::in | std::fstream::binary);
  
  //want these to be unsigned, but idk
  char idLen, cMapType, imgType;
  char cMapSpec[5];
  char imgSpec[10];
  file.get(idLen);
  file.get(cMapType);
  file.get(imgType);
  file.read(cMapSpec, 5);
  file.read(imgSpec, 10);
  //read header
  if (imgType == 0) return timage(); //no img data
  if (cMapType == 1) {
    //do color map reading
    //TODO
    file.close();
    return timage();
  } else {
    if (imgType == 1 || imgType >= 9) {
      //TODO
      //color mapped or RLE'd
      file.close();
      return timage();
    }
    //direct data
    char& bbp = imgSpec[8];
    uint16_t& fwidth = *((uint16_t*)&imgSpec[4]);
    uint16_t& fheight = *((uint16_t*)&imgSpec[6]);

    timage out = timage(fwidth, fheight);
    char data[4];
    uint16_t sdata;
    uint8_t r,g,b;
    for (int y = 0; y < fheight; y++) {
      for (int x = 0; x < fwidth; x++) {
        switch (bbp) {
          case 8:
            file.get(*data);
            out.pixels[x + y*fwidth] = tcolor(data[0], data[0], data[0]);
            break;
          case 15:
          case 16:
            file.read(data,2);
            sdata = *((uint16_t*)data);
            b = sdata & 0x1F;
            g = (sdata >> 5) & 0x1F;
            r = (sdata >> 10) & 0x1F;
            out.pixels[x + y*fwidth] = tcolor(r,g,b);
            break;
          case 24:
            file.read(data,3);
            out.pixels[x + y*fwidth] = tcolor(data[2], data[1], data[0]);
            break;
          case 32:
            file.read(data,4);
            out.pixels[x + y*fwidth] = tcolor(data[2], data[1], data[0]);
            break;
        }
      }
    }
    file.close();
    return out;
  }
}

tcolor timage::get(int x, int y) const {
  if (x < 0 || x >= width || y < 0 || y >= height) return tcolor(0,0,0);
  return pixels[y*width + x];
}

void timage::set(int x, int y, tcolor v) {
  pixels[y*width + x] = v;
}
