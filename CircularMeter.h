//guard 

#ifndef CIRCULARMETER_H
#define CIRCULARMETER_H

#include "Meter.h"


//class circular meter that inherits from meter and has a few new variables
//radius, boolean colourful
class CircularMeter : public Meter
{
public:
    CircularMeter(int x, int y, int value,const char* text, int radius, TFT_eSPI* screen , TFT_eSprite* sprite, OpenFontRender* ofr, bool colourful = false);
    void draw();
    void set_radius(int radius);
    int get_radius();
    void set_colourful(bool colourful);
    bool get_colourful();
private:
    int radius;
    bool colourful;
    int last_angle;
};

#endif // !CIRCULARMETER_H