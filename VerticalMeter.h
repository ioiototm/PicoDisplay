//guard

#ifndef VERTICALMETER_H
#define VERTICALMETER_H


#include "Meter.h"

//class vertical meter that has added max_w and max_h variables
class VerticalMeter : public Meter
{
public:
    VerticalMeter(int x, int y, int value, const char* text, int max_w, int max_h, TFT_eSPI* screen, TFT_eSprite* sprite, OpenFontRender* ofr);
    void draw();
    void set_max_w(int max_w);
    void set_max_h(int max_h);
    int get_max_w();
    int get_max_h();
private:
    int max_w;
    int max_h;
};


#endif