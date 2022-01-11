#include <iostream>
#include <string>

//DRM declartions
//and my covers for them

extern unsigned int SWIDTH, SHEIGHT;

int fbSetup(std::string);
void fbCleanup(void);
void flip(void);
void setPixel(unsigned int, unsigned int, unsigned char, unsigned char, unsigned char);

//start stolen stuff

struct modeset_buf;
struct modeset_dev;
