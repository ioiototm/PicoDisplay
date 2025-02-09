#include "RGBManager.h"
#include "Utilities.h"



RGBManager::RGBManager(TFT_eSPI* screen, RGB_EFFECT effect, int speed, int width, int height) {
  this->screen = screen;
  this->effect = effect;
  this->color = 0;
  this->speed = speed;
  this->direction = 0;
  this->width = width;
  this->height = height;
  this->x = 0;
  this->y = 0;
  this->speedCounter = 0;
}

void RGBManager::draw() {

  if (RGB_EFFECT::NONE == this->effect) {
    if (updated) {

       for (int i = 0; i < width; i++) {
      screen->drawRect(x + i, y + i, screen->width() - (i * 2), screen->height() - (i * 2), TFT_BLACK);
    }


      updated = false;
    }
    return;
  }

  //if single color and not updated, skip
  if (RGB_EFFECT::SINGLE_COLOR == this->effect && !updated) {
    return;
  }

  speedCounter++;

  if (speedCounter == speed) {
    switch (this->effect) {
      case RGB_EFFECT::SINGLE_COLOR:
        single_color();
        break;
      case RGB_EFFECT::RAINBOW_LINE:
        rainbow_line();
        break;
      case RGB_EFFECT::RAINBOW_SNAKE:
        //rainbow_snake();
        break;
      default:
        break;
    }

    speedCounter = 0;

    //calculate next position
    //the line should go around the screen at the edge
    //from top left, top right, bottom right, bottom left
    //then back to top left

    //top left
    if (x == 0 && y == 0) {
      x = 0;
      y = 0;
      direction = 0;
    }
    //top right
    else if (x == screen->width() - width && y == 0) {
      x = screen->width() - width;
      y = 0;
      direction = 1;
    }
    //bottom right
    else if (x == screen->width() - width && y == screen->height() - height) {
      x = screen->width() - width;
      y = screen->height() - height;
      direction = 2;
    }
    //bottom left
    else if (x == 0 && y == screen->height() - height) {
      x = 0;
      y = screen->height() - height;
      direction = 3;
    }
  }
}

void RGBManager::single_color() {
  if (updated) {

    //based on width, draw a rectangle with that thickness by going through for loop and drawing a rectangle for each width
    //so if width is 4, draw 4 rectangles
    //first at 0,0 with width and height of screen width and height
    //second at 1,1 with width and height of screen width - 2 and height - 2
    //third at 2,2 with width and height of screen width - 4 and height - 4
    //etc

    for (int i = 0; i < width; i++) {
      screen->drawRect(x + i, y + i, screen->width() - (i * 2), screen->height() - (i * 2), color);
    }


    //depening on width and height, draw a rectangle around the screen with the set thickness of width and height
    //using two for loops, one for the width and one for the height
    //draw a rectangle at the current x and y position (line by line)

    // //draw the top and bottom lines with the height as thickness
    // //using the drawLine function
    // //call it height times

    // for(int i = 0; i < height; i++){
    //   screen->drawLine(x, y + i, screen->width() - width, y + i, color);
    //   screen->drawLine(x, screen->height() - height + i, screen->width() - width, screen->height() - height + i, color);
    // }

    // //draw the left and right lines with the width as thickness
    // //using the drawLine function
    // //call it width times
    // for(int i = 0; i < width; i++){
    //   screen->drawLine(x + i, y, x + i, screen->height() - height, color);
    //   screen->drawLine(screen->width() - width + i, y, screen->width() - width + i, screen->height() - height, color);
    // }

    //screen->drawRect(x, y, screen->width() - width, screen->height() - height, color);
    updated = false;
  }
}

void RGBManager::rainbow_line() {
  //if (updated)
  {
    color = cycleRainbow(color);

    for (int i = 0; i < width; i++) {
      screen->drawRect(x + i, y + i, screen->width() - (i * 2), screen->height() - (i * 2), color);
    }
    updated = false;
  }
}


void RGBManager::set_effect(RGB_EFFECT effect) {
  this->effect = effect;
  updated = true;
}

RGB_EFFECT RGBManager::get_effect() {
  return this->effect;
}

void RGBManager::set_color(uint16_t color) {
  this->color = color;
  updated = true;
}

uint16_t RGBManager::get_color() {
  return this->color;
}

void RGBManager::set_speed(int speed) {
  this->speed = speed;
  updated = true;
  speedCounter = 0;
}

int RGBManager::get_speed() {
  return this->speed;
}

void RGBManager::set_direction(int direction) {
  this->direction = direction;
}

int RGBManager::get_direction() {
  return this->direction;
}

void RGBManager::set_width(int width) {
  this->width = width;
  updated = true;
}

int RGBManager::get_width() {
  return this->width;
}

void RGBManager::set_height(int height) {
  this->height = height;
  updated = true;
}

int RGBManager::get_height() {
  return this->height;
}
