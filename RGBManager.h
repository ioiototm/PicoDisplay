//guard 
#ifndef RGBMANAGER_H
#define RGBMANAGER_H

//an RGB manager class that would be the rgb effects around the screen, it accepts an TFT_eSPI object in the constructor
//it has a draw function that draws the rgb effects around the screen
//enum for the different effects

class TFT_eSPI;
#include <stdint.h>

//enum class for colour rainbow cycle
//single_color
//rainbow_line
//rainbow_snake
//none

enum class RGB_EFFECT
{
    SINGLE_COLOR,
    RAINBOW_LINE,
    RAINBOW_SNAKE,
    NONE
};

class RGBManager
{
public:
    RGBManager(TFT_eSPI* screen, RGB_EFFECT effect, int speed = 100, int width = 4, int height = 4);
    void draw();
    void set_effect(RGB_EFFECT effect);
    RGB_EFFECT get_effect();
    void set_color(uint16_t color);
    uint16_t get_color();
    void set_speed(int speed);
    int get_speed();
    void set_direction(int direction);
    int get_direction();
    void set_width(int width);
    int get_width();
    void set_height(int height);
    int get_height();
private:

    void single_color();
    void rainbow_line();
    void rainbow_snake();

    RGB_EFFECT effect;
    uint16_t color;
    int speed;
    int direction;
    int width;
    int height;
    int x,y;
    bool updated = false;
    int speedCounter;

    TFT_eSPI* screen;
};

#endif // !RGBMANAGER_H