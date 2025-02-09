//guard

#ifndef METER_H
#define METER_H

class TFT_eSPI;
class TFT_eSprite;
class OpenFontRender;


//class meter that is a virtual class, has x y coordinates, a value, a last_value, text
class Meter
{
public:
    Meter(int x, int y, int value, const char* text, TFT_eSPI* screen, TFT_eSprite* sprite, OpenFontRender* ofr);
    virtual void draw() = 0;
    virtual void set_value(int value);
    virtual void set_x(int x);
    virtual void set_y(int y);
    virtual int get_value();
    virtual int get_last_value();
    virtual const char* get_text();
    virtual int get_x();
    virtual int get_y();

protected:
    int xpos;
    int ypos;
    int value;
    int last_value;
    const char* text;
    bool initMeter = true;

    TFT_eSPI* screen;
    TFT_eSprite* sprite;
    OpenFontRender* ofr;
    
    
};


#endif // !METER_H
