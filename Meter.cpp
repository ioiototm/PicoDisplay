#include "Meter.h"

Meter::Meter(int x, int y, int value, const char* text, TFT_eSPI* screen, TFT_eSprite* sprite, OpenFontRender* ofr)
{
    this->xpos = x;
    this->ypos = y;
    this->value = value;
    this->last_value = 0;
    this->text = text;
    this->screen = screen;
    this->sprite = sprite;
    this->ofr = ofr;
    this->initMeter = true;

}

//set the value of the meter
void Meter::set_value(int value)
{
    this->value = value;
}


//set the x coordinate of the meter
void Meter::set_x(int x)
{
    this->xpos = x;
}

//set the y coordinate of the meter
void Meter::set_y(int y)
{
    this->ypos = y;
}

//get the value of the meter
int Meter::get_value()
{
    return this->value;
}

//get the last value of the meter
int Meter::get_last_value()
{
    return this->last_value;
}

//get the text of the meter
const char* Meter::get_text()
{
    return this->text;
}

//get the x coordinate of the meter
int Meter::get_x()
{
    return this->xpos;
}

//get the y coordinate of the meter
int Meter::get_y()
{
    return this->ypos;
}


