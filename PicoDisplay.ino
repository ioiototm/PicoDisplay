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

#ifdef DRAW_DIGITS
#include "NotoSans_Bold.h"
#include "OpenFontRender.h"
#define TTF_FONT NotoSans_Bold
#endif




TFT_eSPI tft = TFT_eSPI();            // Invoke custom library with default width and height
TFT_eSprite spr = TFT_eSprite(&tft);  // Declare Sprite object "spr" with pointer to "tft" object

void sleep(bool value)
{
    if (value)
    {
        tft.writecommand(0x10);   // Send command to put the display to sleep.
        delay(150);           // Delay for shutdown time before another command can be sent.
    }
    else
    {
        tft.writecommand(0x11);               // This sends the wake up command and initialises the display
	      delay(50);            // Extra delay to stop a "white flash" while the TFT is initialising.
    }
}


#ifdef DRAW_DIGITS
OpenFontRender ofr;
#endif

uint32_t runTime = 0;  // time for next update

void setup(void) {

  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);


  for(int i = 30; i<330;i++)
  {
    int index = i-30;
    rainbowColours[index] = rainbow(map(i, 30, 330, 70, 0));
  }

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
  ofr.setCursor(20, 0);
  ofr.cprintf("CPU");

 

  CircularMeter cpuDegrees(90, 80, 20, "C", 70,&tft,&spr,&ofr);
  CircularMeter cpuUsage(240, 80, 50, "Use",70,&tft,&spr,&ofr);
 
  ofr.setCursor(20, 160);
  ofr.cprintf("GPU");

  ofr.setCursor(320, 0);
  ofr.cprintf("RAM");

  VerticalMeter ramUsage(350,7,99, "test",100,140,&tft,&spr,&ofr);
  CircularMeter gpuDegrees(90, 245, 50, "C",70,&tft,&spr,&ofr);
  



  String incomingData;

  while (true) {
    if (millis() - runTime >= LOOP_DELAY) {


      if (Serial.available()) {
        incomingData = Serial.readStringUntil('\n');
        //Serial.println("message was" + test);

        if(incomingData.indexOf("SLEEP") != -1)
        {
          sleep(true);
          continue;
        }
        if(incomingData.indexOf("WAKEUP") != -1)
        {
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
      
    }
  }

#ifdef DRAW_DIGITS
  ofr.unloadFont();  // Recover space used by font metrics etc
#endif
}

