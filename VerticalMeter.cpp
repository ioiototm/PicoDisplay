#include "VerticalMeter.h"

#include "Utilities.h"

#define DRAW_DIGITS 

#ifdef DRAW_DIGITS
#include "NotoSans_Bold.h"
#include "OpenFontRender.h"
#define TTF_FONT NotoSans_Bold
#endif

VerticalMeter::VerticalMeter(int x, int y, int value, const char* text, int max_w, int max_h, TFT_eSPI* screen, TFT_eSprite* sprite, OpenFontRender* ofr) : Meter(x, y, value, text, screen, sprite, ofr)
{
    this->max_w = max_w;
    this->max_h = max_h;
}

//draw the vertical meter
void VerticalMeter::draw()
{
    if (initMeter) {
      //tft.fillRoundRect(xpos+5, ypos+5, max_w-10, max_h-10, 10, TFT_SILVER);
      screen->fillRect(xpos + 3, ypos + 3, max_w - 6, max_h - 6, DARKER_GREY);
      //tft.drawSmoothRoundRect(xpos, ypos, 10, 0 , max_w, max_h, TFT_SILVER, DARKER_GREY);
      screen->drawSmoothRoundRect(xpos, ypos, 2, 0, max_w, max_h, TFT_WHITE, DARKER_GREY);

      int digit = value;

      int heightOfSegment = max_h / 10;

      for (int i = 0; digit > 10; i++, digit -= 10) {
       
        screen->drawSmoothRoundRect(xpos + 6, ypos + max_h - 19 - heightOfSegment * i, 5, 0, max_w - 12, heightOfSegment - 10, rainbow(map(i, 0, 10, 50, 0)), DARKER_GREY);
      }


      initMeter = false;
    }

    if (last_value != value) {
      screen->fillRect(xpos + 3, ypos + 3, max_w - 6, max_h - 6, DARKER_GREY);
      int digit = value;

      int heightOfSegment = max_h / 10;

      for (int i = 0; digit > 10; i++, digit -= 10) {
        
        screen->drawSmoothRoundRect(xpos + 6, ypos + max_h - 19 - heightOfSegment * i, 5, 0, max_w - 12, heightOfSegment - 10, rainbow(map(i, 0, 10, 50, 0)), DARKER_GREY);
      }



#ifdef DRAW_DIGITS
      ofr->setDrawer(*sprite);  // Link renderer to sprite (font will be rendered in sprite spr)

      // Add value in centre if radius is a reasonable size
      //if (radius >= 25)
      {
        // This code gets the font dimensions in pixels to determine the required the sprite size
        ofr->setFontSize((40));
        //ofr.setFontColor(rainbow(map(reading, 0, 100, 50, 0)), DARKER_GREY);
        ofr->setFontColor(TFT_WHITE,DARKER_GREY);

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
        sprite->fillSprite(TFT_BLACK);                          // (TFT_BLUE); // (DARKER_GREY);
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
        sprite->pushSprite(xpos - w - 1, ypos + h + 8);  // Push sprite containing the val number
        sprite->deleteSprite();                      // Recover used memory
      }
#endif
    }
    last_value = value;
}