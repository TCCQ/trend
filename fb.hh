#include <string>

//covers for BOTH 
//drm and sdl backends
//choose at compiletime

extern unsigned int SWIDTH, SHEIGHT;

int fbSetup(std::string);
void fbCleanup(void);
void flip(void);
void setPixel(unsigned int, unsigned int, unsigned char, unsigned char, unsigned char);
