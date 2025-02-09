//guard

#ifndef UTILITIES_H
#define UTILITIES_H


#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library
#include <string>


#define DARKER_GREY 0x18E3

static unsigned int rainbowColours[300];

inline uint16_t hexStringToRgb565(const char* hexString) {
    uint16_t colorValue = 0;

    for (int i = 0; i < 4; ++i) {
        char ch = std::tolower(hexString[i]);
        colorValue <<= 4;
        if (ch >= '0' && ch <= '9') {
            colorValue |= (ch - '0');
        } else if (ch >= 'a' && ch <= 'f') {
            colorValue |= (ch - 'a' + 10);
        } else {
            // Invalid character encountered
            return 0;  // Or handle the error as needed
        }
    }

    return colorValue;
}

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

// Function to convert RGB565 to HSV
inline void rgb565ToHsv(uint16_t rgb565, float& hue, float& saturation, float& value) {
  // Extract RGB components from the 565 format
  uint8_t red = (rgb565 >> 11) & 0x1F;
  uint8_t green = (rgb565 >> 5) & 0x3F;
  uint8_t blue = rgb565 & 0x1F;

  // Convert RGB to normalized float values (0.0 - 1.0)
  float r = static_cast<float>(red) / 31.0f;
  float g = static_cast<float>(green) / 63.0f;
  float b = static_cast<float>(blue) / 31.0f;

  // Find the maximum and minimum component values
  float maxVal = std::max(std::max(r, g), b);
  float minVal = std::min(std::min(r, g), b);

  // Calculate value (brightness)
  value = maxVal;

  // Calculate saturation
  saturation = (maxVal > 0.0f) ? ((maxVal - minVal) / maxVal) : 0.0f;

  // Calculate hue
  if (maxVal == minVal) {
    // Monochromatic, no hue
    hue = 0.0f;
  } else {
    float delta = maxVal - minVal;
    if (maxVal == r) {
      hue = (g - b) / delta + (g < b ? 6.0f : 0.0f);
    } else if (maxVal == g) {
      hue = (b - r) / delta + 2.0f;
    } else {  // maxVal == b
      hue = (r - g) / delta + 4.0f;
    }
    hue /= 6.0f;
  }
}

// Function to convert HSV to RGB565
inline uint16_t hsvToRgb565(float hue, float saturation, float value) {
  // Calculate RGB components in the range 0-1
  float r, g, b;
  if (saturation == 0.0f) {
    // Monochromatic, grayscale
    r = g = b = value;
  } else {
    hue *= 6.0f;
    int i = static_cast<int>(hue);
    float f = hue - i;
    float p = value * (1.0f - saturation);
    float q = value * (1.0f - saturation * f);
    float t = value * (1.0f - saturation * (1.0f - f));
    switch (i) {
      case 0:
        r = value;
        g = t;
        b = p;
        break;
      case 1:
        r = q;
        g = value;
        b = p;
        break;
      case 2:
        r = p;
        g = value;
        b = t;
        break;
      case 3:
        r = p;
        g = q;
        b = value;
        break;
      case 4:
        r = t;
        g = p;
        b = value;
        break;
      default:  // case 5:
        r = value;
        g = p;
        b = q;
        break;
    }
  }

  // Convert normalized float values (0.0 - 1.0) to RGB565 format
  uint8_t red = static_cast<uint8_t>(r * 31.0f + 0.5f);
  uint8_t green = static_cast<uint8_t>(g * 63.0f + 0.5f);
  uint8_t blue = static_cast<uint8_t>(b * 31.0f + 0.5f);

  // Pack RGB565 components into a single 16-bit value
  return (red << 11) | (green << 5) | blue;
}

// Function to cycle through the rainbow colors
inline uint16_t cycleRainbow(uint16_t rgb565) {
  float hue, saturation, value;
  rgb565ToHsv(rgb565, hue, saturation, value);

  // Increment the hue component
  hue += 1.0f / 360.0f;  // Assuming 360 degrees for the hue range

  // Wrap the hue value if it goes beyond 1.0
  if (hue > 1.0f) {
    hue -= 1.0f;
  }

  // Convert back to RGB565
  return hsvToRgb565(hue, saturation, value);
}


#endif // !UTILITIES_H