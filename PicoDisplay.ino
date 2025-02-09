// This is a test sketch being developed for a new arc based meter widget
// The meter grahic is fully anti-aliased to avoid jaggy pixelated edges

// For this demo randomly sized meters are drawn, cycled and redrawn a random size.
// The meter is ramped up and down 0-100 and 100-0, then pauses before a new
// random sized meter is drawn

// If the radius is > 25 then the value is drawn in the middle

// The outer ring of the meter uses the drawSmoothCircle function (which draws
// a narrow full circle smooth arc)

// Uncomment to draw meter digits and label text
//#define DRAW_DIGITS

// If DRAW_DIGITS is defined the OpenFontRender library must be loaded since
// the sketch uses a scaleable TrueType font for the text and numerals.
// https://github.com/Bodmer/OpenFontRender

#define LOOP_DELAY 0  // This controls how frequently the meter is updated \
                      // for test purposes this is set to 0


#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library

#define DRAW_DIGITS

#include "Utilities.h"

#include "CircularMeter.h"
#include "VerticalMeter.h"
#include "RGBManager.h"

#ifdef DRAW_DIGITS
#include "NotoSans_Bold.h"
#include "OpenFontRender.h"
#define TTF_FONT NotoSans_Bold
#endif



//enum for colour rainbow cycle
//single_color
//rainbow_line
//rainbow_snake

enum color_mode {
  single_color,
  rainbow_line,
  rainbow_snake,
  none
};



TFT_eSPI tft = TFT_eSPI();            // Invoke custom library with default width and height
TFT_eSprite spr = TFT_eSprite(&tft);  // Declare Sprite object "spr" with pointer to "tft" object

void sleep(bool value) {
  if (value) {
    tft.writecommand(0x10);  // Send command to put the display to sleep.
    delay(150);              // Delay for shutdown time before another command can be sent.
  } else {
    tft.writecommand(0x11);  // This sends the wake up command and initialises the display
    delay(50);               // Extra delay to stop a "white flash" while the TFT is initialising.
  }
}


#ifdef DRAW_DIGITS
OpenFontRender ofr;
#endif

uint32_t runTime = 0;  // time for next update
uint16_t counter = 0;





color_mode currentMode = none;

void setup(void) {

  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);


  for (int i = 30; i < 330; i++) {
    int index = i - 30;
    rainbowColours[index] = rainbow(map(i, 30, 330, 70, 0));
  }
}

int colourSpeed = 1000;
int counterForRect = 0;

//function that draws the rainbow based on the currentMode
void drawRainbow() {



  switch (currentMode) {

    case none:
      break;
    case rainbow_snake:



      static int x = 0, y = 0;
      static uint16_t currColour = TFT_RED;

      //the following code draws a line that moves across the screen
      //it goes at the edges of the screen, around the screen
      //every loop it draws a pixel, then moves the pixel

      if (counterForRect == colourSpeed) {
        //tft.drawPixel(x, y, rainbow(map(x,0,479,0,127)));

        currColour = cycleRainbow(currColour);

        tft.fillRect(x, y, 4, 4, currColour);
        //draw it on the opposite side of the screen as well
        tft.fillRect(476 - x, 316 - y, 4, 4, currColour);

        if (x < 476 && y == 0) {
          x++;
        } else if (x == 476 && y < 316) {
          y++;
        } else if (x > 0 && y == 316) {
          x--;
        } else if (x == 0 && y > 0) {
          y--;
        }

        counterForRect = 0;
      }

      break;
  }



  counterForRect++;
}

#include <ctype.h>  // for isdigit() and isspace()

#include <stdlib.h>  // for atof()
#include <string.h>  // for strstr() and strcpy()

// Helper function: splits an input speed string into numeric and unit parts.
// For example, "86.0Mbps" becomes "86.0" in numPart and "Mbps" in unitPart.
void splitSpeed(const char* input, char* numPart, char* unitPart) {
  int i = 0, j = 0;
  // Skip leading whitespace
  while (input[i] && isspace(input[i])) { i++; }
  // Copy numeric characters (digits and '.')
  while (input[i] && (isdigit(input[i]) || input[i] == '.')) {
    numPart[j++] = input[i++];
  }
  numPart[j] = '\0';
  // Skip any whitespace between the number and the unit
  while (input[i] && isspace(input[i])) { i++; }
  // Copy the rest into unitPart
  strcpy(unitPart, input + i);
}

void updateInternetSpeed(const char* downloadSpeed, const char* uploadSpeed) {
  // Switch to control whether text itself uses threshold colors.
  // Set to true to color the text, false to keep it white.
  bool enableTextColor = false; 

  // Set the desired font size.
  ofr.setFontSize(50);

  // Define a template string for the full speed display.
  // This helps us determine the overall sprite width.
  const char* templateStr = "999.9 Mbps";  // Adjust if needed
  uint16_t fixedSpriteWidth = ofr.getTextWidth(templateStr) ;  // a couple extra pixels for margin

  // Calculate line height and total sprite height.
  uint16_t lineHeight = ofr.getTextHeight("A") + 4; // a basic line height with a little extra
  uint16_t fixedSpriteHeight = (lineHeight * 2) + 2;  // two lines with a tiny margin

  // Create a sprite with fixed dimensions.
  TFT_eSprite speedSprite = TFT_eSprite(&tft);
  speedSprite.createSprite(fixedSpriteWidth, fixedSpriteHeight + 9);
  speedSprite.fillSprite(TFT_BLACK);  // Clear with the background color

  // Direct the font renderer to our sprite.
  ofr.setDrawer(speedSprite);

  // Split the download and upload strings into numeric and unit parts.
  char dNum[20], dUnit[10];
  char uNum[20], uUnit[10];
  splitSpeed(downloadSpeed, dNum, dUnit);
  splitSpeed(uploadSpeed, uNum, uUnit);

  // Determine a fixed x-position for printing the unit part.
  // For this, we use a numeric template representing the maximum expected numeric part.
  const char* numericTemplate = "999.9";  // adjust if needed
  uint16_t numericTemplateWidth = ofr.getTextWidth(numericTemplate);
  int fixedUnitX = numericTemplateWidth + 18;  // leave some gap between the number and the unit

  // --- Colour Thresholds for arrows (and optionally text) ---
  // For download, convert the numeric part to a float in Mbps.
  float dSpeedMbps = atof(dNum);
  if (strstr(dUnit, "kbps") || strstr(dUnit, "Kbps")) {
    dSpeedMbps /= 1000.0;  // Convert kbps to Mbps
  }
  uint16_t downloadColor;
  if (dSpeedMbps < 10.0) {
    downloadColor = TFT_GREEN;
  } else if (dSpeedMbps < 40.0) {
    downloadColor = TFT_YELLOW;
  } else {
    downloadColor = TFT_RED;
  }

  // For upload, convert the numeric part to a float in Mbps.
  float uSpeedMbps = atof(uNum);
  if (strstr(uUnit, "kbps") || strstr(uUnit, "Kbps")) {
    uSpeedMbps /= 1000.0;  // Convert kbps to Mbps
  }
  uint16_t uploadColor;
  if (uSpeedMbps < 5.0) {
    uploadColor = TFT_GREEN;
  } else if (uSpeedMbps < 15.0) {
    uploadColor = TFT_YELLOW;
  } else {
    uploadColor = TFT_RED;
  }

  // --- Print the download speed (first line) ---
  int downloadNumX = 25;  // numeric part on the left (adjust as needed)
  int downloadY = 1;      // small vertical margin at the top

  // Set text color based on flag.
  if (enableTextColor) {
    ofr.setFontColor(downloadColor, TFT_BLACK);
  } else {
    ofr.setFontColor(TFT_WHITE, TFT_BLACK);
  }
  ofr.setCursor(downloadNumX, downloadY);
  ofr.cprintf(dNum);

  // Print the unit part always in white.
  ofr.setFontColor(TFT_WHITE, TFT_BLACK);
  ofr.setCursor(fixedUnitX, downloadY);
  ofr.cprintf(dUnit);

  // --- Print the upload speed (second line) ---
  int uploadNumX = 25;  // numeric part left-aligned
  int uploadY = downloadY + lineHeight;  // next line down

  if (enableTextColor) {
    ofr.setFontColor(uploadColor, TFT_BLACK);
  } else {
    ofr.setFontColor(TFT_WHITE, TFT_BLACK);
  }
  ofr.setCursor(uploadNumX, uploadY);
  ofr.cprintf(uNum);

  // Print the unit part always in white.
  ofr.setFontColor(TFT_WHITE, TFT_BLACK);
  ofr.setCursor(fixedUnitX, uploadY);
  ofr.cprintf(uUnit);

  // --- Draw static arrows on the right side ---
  // We'll draw a down arrow for the download line and an up arrow for the upload line.
  // Let's make them a bit bigger.
  int arrowWidth = 20, arrowHeight = 15;
  int arrowMarginRight = 5; // distance from the right edge of the sprite
  int arrowX = fixedSpriteWidth - arrowMarginRight - arrowWidth + 4;
  
  // Adjust arrow vertical positions (tweak as needed for your layout).
  int arrowY_download = downloadY + (lineHeight - arrowHeight) / 2 + 7;
  int arrowY_upload   = uploadY + (lineHeight - arrowHeight) / 2 + 11;
  
  // Draw a downward arrow on the download line.
  speedSprite.fillTriangle(arrowX, arrowY_download,
                           arrowX + arrowWidth, arrowY_download,
                           arrowX + arrowWidth/2, arrowY_download + arrowHeight,
                           downloadColor);
  
  // Draw an upward arrow on the upload line.
  speedSprite.fillTriangle(arrowX + arrowWidth/2, arrowY_upload,
                           arrowX, arrowY_upload + arrowHeight,
                           arrowX + arrowWidth, arrowY_upload + arrowHeight,
                           uploadColor);

  // Push the sprite onto the main display at your chosen location.
  speedSprite.pushSprite(175, 210);

  // Clean up by deleting the sprite and resetting the drawer.
  speedSprite.deleteSprite();
  ofr.setDrawer(tft);
}


void loop() {

#ifdef DRAW_DIGITS
  // Loading a font takes a few milliseconds, so for test purposes it is done outside the test loop
  if (ofr.loadFont(TTF_FONT, sizeof(TTF_FONT))) {
    Serial.println("Render initialize error");
    return;
  }
#endif



  // Make the TFT the print destination, print the units label direct to the TFT
  ofr.setDrawer(tft);
  ofr.setFontColor(TFT_WHITE, TFT_BLACK);
  ofr.setFontSize(40);
  ofr.setCursor(24, 0);
  ofr.cprintf("CPU");



  CircularMeter cpuDegrees(90, 80, 20, "C", 70, &tft, &spr, &ofr);
  CircularMeter cpuUsage(240, 80, 50, "Use", 70, &tft, &spr, &ofr);

  ofr.setCursor(24, 160);
  ofr.cprintf("GPU");

  ofr.setCursor(320, 0);
  ofr.cprintf("RAM");

  ofr.setCursor(315, 158);
  ofr.cprintf("VRAM");

  ofr.setCursor(240, 185);
  ofr.cprintf("Internet");

  VerticalMeter ramUsage(350, 7, 99, "test", 100, 140, &tft, &spr, &ofr);
  CircularMeter gpuDegrees(90, 240, 50, "C", 70, &tft, &spr, &ofr);

  VerticalMeter vramUsage(350,165,99,"test2",100,140,&tft,&spr, &ofr);

  RGBManager rgbManager(&tft, RGB_EFFECT::SINGLE_COLOR);
  rgbManager.set_color(TFT_RED);


  String downloadSpeed = "0 kbps";
  String uploadSpeed = "0 kbps";


  String incomingData;
  //start the drawRect function on the second core
  //multicore_launch_core1(drawRect);
  //drawRect();

  while (true) {
    if (millis() - runTime >= LOOP_DELAY) {


      if (Serial.available()) {
        incomingData = Serial.readStringUntil('\n');
        //Serial.println("message was" + test);

        if (incomingData.indexOf("SLEEP") != -1) {
          sleep(true);
          continue;
        }
        if (incomingData.indexOf("WAKEUP") != -1) {
          sleep(false);
          continue;
        }

        String token;

        uint8_t token_idx = 0;
        while (get_token(incomingData, token, token_idx, ';')) {



          if (token.indexOf("CPU") != -1) {
            String innerToken;
            uint8_t innerToken_idx = 0;
            while (get_token(token, innerToken, innerToken_idx, ':')) {
              if (innerToken_idx == 1) {
                cpuDegrees.set_value(innerToken.toInt());
              } else if (innerToken_idx == 2) {
                cpuUsage.set_value(innerToken.toInt());
              }
              //Serial.println(innerToken);
              innerToken_idx++;
            }
          }

          else if (token.indexOf("RAM") != -1) {
            String innerToken;
            uint8_t innerToken_idx = 0;
            while (get_token(token, innerToken, innerToken_idx, ':')) {
              if (innerToken_idx == 1) {
                ramUsage.set_value(innerToken.toInt());
              }
              //Serial.println(innerToken);
              innerToken_idx++;
            }
          }else if(token.indexOf("VMEM") != -1)
          {
            String innerToken;
            uint8_t innerToken_idx = 0;
            while (get_token(token, innerToken, innerToken_idx, ':')) {
              if (innerToken_idx == 1) {
                vramUsage.set_value(innerToken.toInt());
              }
              //Serial.println(innerToken);
              innerToken_idx++;
            }
          } 
          else if (token.indexOf("GPU") != -1) {
            String innerToken;
            uint8_t innerToken_idx = 0;
            while (get_token(token, innerToken, innerToken_idx, ':')) {
              if (innerToken_idx == 1) {
                gpuDegrees.set_value(innerToken.toInt());
              }
              //Serial.println(innerToken);
              innerToken_idx++;
            }
          }else if(token.indexOf("DS") != -1)
          {
            //download speed
            String innerToken;
            uint8_t innerToken_idx = 0;
            while (get_token(token, innerToken, innerToken_idx, ':')) {
              if (innerToken_idx == 1) {
                downloadSpeed = innerToken;
              }
              //Serial.println(innerToken);
              innerToken_idx++;
            } 
          }else if(token.indexOf("US") != -1)
          {
            //upload speed
            String innerToken;
            uint8_t innerToken_idx = 0;
            while (get_token(token, innerToken, innerToken_idx, ':')) {
              if (innerToken_idx == 1) {
                uploadSpeed = innerToken;
              }
              //Serial.println(innerToken);
              innerToken_idx++;
            }
          }
          
          else if (token.indexOf("COLOR_SPEED") != -1) {
            String innerToken;
            uint8_t innerToken_idx = 0;
            while (get_token(token, innerToken, innerToken_idx, ':')) {
              if (innerToken_idx == 1) {
                rgbManager.set_speed(innerToken.toInt());

              }
              //Serial.println(innerToken);
              innerToken_idx++;
            }
          } else if (token.indexOf("COLOR_MODE") != -1) {
            String innerToken;
            uint8_t innerToken_idx = 0;
            while (get_token(token, innerToken, innerToken_idx, ':')) {
              if (innerToken_idx == 1) {
                //the innerToken is the color mode in string form with capital letters
                //we need to convert it to the enum

                //Serial.println(innerToken);
                if (innerToken.indexOf("RAINBOW_SNAKE") != -1) {
                  rgbManager.set_effect(RGB_EFFECT::RAINBOW_SNAKE);
                } else if (innerToken.indexOf("RAINBOW_LINE") != -1) {
                  rgbManager.set_effect(RGB_EFFECT::RAINBOW_LINE);
                } else if (innerToken.indexOf("SINGLE_COLOR") != -1) {
                  rgbManager.set_effect(RGB_EFFECT::SINGLE_COLOR);
                } else if (innerToken.indexOf("NONE") != -1) {
                  rgbManager.set_effect(RGB_EFFECT::NONE);
                }
              }
              //Serial.println(innerToken);
              innerToken_idx++;
            }

        

          }  else if (token.indexOf("COLOR_SIZE") != -1) {
            String innerToken;
            uint8_t innerToken_idx = 0;
            while (get_token(token, innerToken, innerToken_idx, ':')) {
              if (innerToken_idx == 1) {//width
                rgbManager.set_width(innerToken.toInt());
              } else if (innerToken_idx == 2) {//height
                rgbManager.set_height(innerToken.toInt());
              }
              //Serial.println(innerToken);
              innerToken_idx++;
            }
          } else if (token.indexOf("COLOR") != -1) {
            String innerToken;
            uint8_t innerToken_idx = 0;
            while (get_token(token, innerToken, innerToken_idx, ':')) {
              if (innerToken_idx == 1) {
                rgbManager.set_color(hexStringToRgb565(innerToken.c_str()));
               
              }
              //Serial.println(innerToken);
              innerToken_idx++;
            }
          }

          Serial.println(token);
          token_idx++;
        }
      }

      runTime = millis();
      cpuDegrees.draw();
      cpuUsage.draw();
      ramUsage.draw();
      gpuDegrees.draw();
      vramUsage.draw();

      updateInternetSpeed(downloadSpeed.c_str(), uploadSpeed.c_str());

      rgbManager.draw();
    }
  }

#ifdef DRAW_DIGITS
  ofr.unloadFont();  // Recover space used by font metrics etc
#endif
}
