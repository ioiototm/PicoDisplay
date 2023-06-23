//guard

#ifndef UTILITIES_H
#define UTILITIES_H


#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library

#define DARKER_GREY 0x18E3

static unsigned int rainbowColours[300];

inline unsigned int rainbow(byte value)
{
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = green through to 127 = red

  byte red = 0;    // Red is the top 5 bits of a 16 bit colour value
  byte green = 0;  // Green is the middle 6 bits
  byte blue = 0;   // Blue is the bottom 5 bits

  byte quadrant = value / 32;

  if (quadrant == 0) {
    blue = 0;
    green = 2 * (value % 32);
    red = 31;
  }
  if (quadrant == 1) {
    blue = 0;
    green = 63;
    red = 31 - (value % 32);
  }
  if (quadrant == 2) {
    blue = 0;
    green = 63 - 2 * (value % 32);
    red = 0;
  }
  if (quadrant == 3) {
    blue = 0;
    green = 0;
    red = value % 32;
  }

  return (red << 11) + (green << 5) + blue;
}

inline bool get_token(String &from, String &to, uint8_t index, char separator) {
  uint16_t start = 0, idx = 0;
  uint8_t cur = 0;
  while (idx < from.length()) {
    if (from.charAt(idx) == separator) {
      if (cur == index) {
        to = from.substring(start, idx);
        return true;
      }
      cur++;
      while ((idx < from.length() - 1) && (from.charAt(idx + 1) == separator)) idx++;
      start = idx + 1;
    }
    idx++;
  }
  if ((cur == index) && (start < from.length())) {
    to = from.substring(start, from.length());
    return true;
  }
  return false;
}


#endif // !UTILITIES_H