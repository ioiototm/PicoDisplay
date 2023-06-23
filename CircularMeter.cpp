#include "CircularMeter.h"

#include "Utilities.h"

#define DRAW_DIGITS 

#ifdef DRAW_DIGITS
#include "NotoSans_Bold.h"
#include "OpenFontRender.h"
#define TTF_FONT NotoSans_Bold
#endif


CircularMeter::CircularMeter(int x, int y, int value, const char* text, int radius, TFT_eSPI* screen, TFT_eSprite* sprite, OpenFontRender* ofr, bool colourful) : Meter(x, y, value, text, screen, sprite, ofr)
{
    this->radius = radius;
    this->colourful = colourful;
}

//draw the circular meter
void CircularMeter::draw()
{
    if (initMeter) {
      initMeter = false;
      last_angle = 30;
      screen->fillCircle(xpos, ypos, radius, DARKER_GREY);                    //background of circle
      screen->drawSmoothCircle(xpos, ypos, radius, TFT_SILVER, DARKER_GREY);  //first one - outline
      uint16_t tmp = radius - 3;
      screen->drawArc(xpos, ypos, tmp, tmp - tmp / 5, last_angle, 330, TFT_BLACK, DARKER_GREY);  // meter circle
    }

    radius -= 3;

    // Range here is 0-100 so value is scaled to an angle 30-330
    int val_angle = map(value, 0, 100, 30, 330);


    if (last_angle != val_angle) {
      // Could load the required font here
      //if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT))) {
      //  Serial.println("Render initialize error");
      //  return;
      //}
#ifdef DRAW_DIGITS
      ofr->setDrawer(*sprite);  // Link renderer to sprite (font will be rendered in sprite spr)

      // Add value in centre if radius is a reasonable size
      if (radius >= 25) {
        // This code gets the font dimensions in pixels to determine the required the sprite size
        ofr->setFontSize((6 * radius) / 4);
        ofr->setFontColor(TFT_WHITE, DARKER_GREY);


        // The OpenFontRender library only has simple print functions...
        // Digit jiggle for chaging values often happens with proportional fonts because
        // digit glyph width varies ( 1 narrower that 4 for example). This code prints up to
        // 3 digits with even spacing.
        // A few experiemntal fudge factors are used here to position the
        // digits in the sprite...
        // Create a sprite to draw the digits into
        uint8_t w = ofr->getTextWidth("444");
        uint8_t h = ofr->getTextHeight("4") + 4;
        sprite->createSprite(w, h + 2);
        sprite->fillSprite(DARKER_GREY);                        // (TFT_BLUE); // (DARKER_GREY);
        char str_buf[8];                                    // Buffed for string
        itoa(value, str_buf, 10);                         // Convert value to string (null terminated)
        uint8_t ptr = 0;                                    // Pointer to a digit character
        uint8_t dx = 4;                                     // x offfset for cursor position
        if (value < 100) dx = ofr->getTextWidth("4") / 2;  // Adjust cursor x for 2 digits
        if (value < 10) dx = ofr->getTextWidth("4");       // Adjust cursor x for 1 digit
        while ((uint8_t)str_buf[ptr] != 0) ptr++;           // Count the characters
        while (ptr) {
          ofr->setCursor(w - dx - w / 20, -h / 2.5);  // Offset cursor position in sprtie
          ofr->rprintf(str_buf + ptr - 1);            // Draw a character
          str_buf[ptr - 1] = 0;                      // Replace character with a null
          dx += 1 + w / 3;                           // Adjust cursor for next character
          ptr--;                                     // Decrement character pointer
        }
        sprite->pushSprite(xpos - w / 2, ypos - h / 2);  // Push sprite containing the val number
        sprite->deleteSprite();                          // Recover used memory

        // Make the TFT the print destination, print the units label direct to the TFT
        ofr->setDrawer(*screen);
        ofr->setFontColor(TFT_GOLD, DARKER_GREY);

        int fontSize = radius;
        //if the word is more than 2 characters, make the font smaller
        if (strlen(text) > 2) {
          fontSize = radius - 15;
          Serial.println("old " + String(radius) + " new " + String(fontSize));
        }
        ofr->setFontSize(fontSize);
        ofr->setCursor(xpos, ypos + (radius * 0.4));
        ofr->cprintf(text);
      }
#endif

      //ofr.unloadFont(); // Recover space used by font metrics etc

      // Allocate a value to the arc thickness dependant of radius
      uint8_t thickness = radius / 5;
      if (radius < 25) thickness = radius / 3;

      // Update the arc, only the zone between last_angle and new val_angle is updated


      //tft.drawArc(x, y, r, r - thickness, i, i2, TFT_GREEN, TFT_BLACK); // TFT_SKYBLUE random(0x10000)



      if (val_angle > last_angle) {

    
        if (colourful) {
          for (int i = 30; i < val_angle; i++) {

            int i2 = i;
            i2++;

            int index = i -30;
            screen->drawArc(xpos, ypos, radius, radius - thickness, i, i2, rainbowColours[index], TFT_BLACK);  // TFT_SKYBLUE random(0x10000)
          }
        } else {
          screen->drawArc(xpos, ypos, radius, radius - thickness, 30, val_angle, rainbow(map(val_angle, 30, 330, 70, 0)), TFT_BLACK);
        }
      } else {

        screen->drawArc(xpos, ypos, radius, radius - thickness, 30, val_angle, rainbow(map(val_angle, 30, 330, 70, 0)), TFT_BLACK);
        screen->drawArc(xpos, ypos, radius, radius - thickness, val_angle, last_angle, TFT_BLACK, DARKER_GREY);
      }

     
      last_angle = val_angle;  // Store meter arc position for next redraw
    }

    radius += 3;
}

//set the radius of the meter
void CircularMeter::set_radius(int radius)
{
    this->radius = radius;
}

//get the radius of the meter
int CircularMeter::get_radius()
{
    return this->radius;
}

//set whether it's rainbow coloured or just a single colour (rainbow makes it slower)
void CircularMeter::set_colourful(bool colourful)
{
    this->colourful = colourful;
}

//get whether it's rainbow coloured or just a single colour (rainbow makes it slower)
bool CircularMeter::get_colourful()
{
    return this->colourful;
}




