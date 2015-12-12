#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS (34)

extern uint32_t frame;
extern uint8_t red;
extern uint8_t green;
extern uint8_t blue;
extern Adafruit_NeoPixel strip;

void turnOff() {
  for(uint8_t i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, 0);
  }
}

void endBit() {
  strip.show();
  frame++;
  if (frame >= NUM_PIXELS) {
    frame = 0;
  }
}

// From Adafruit's example
// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino#L121
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void sparkle() {
  turnOff();
  strip.setPixelColor(frame, Wheel(random(255)));
  endBit();
}

void cycle() {
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, Wheel(frame));
  }
  strip.show();
  frame = (frame + 1) & 255;
}

void rainbow() {
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, Wheel(map((i + frame), 0, NUM_PIXELS, 0, 255)));
  }
  endBit();
}

void party() {
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, Wheel(random(255)));
  }
  strip.show();
}

//12 to 24
void cylon() {
  turnOff();
  uint8_t pixel = frame;
  if(pixel > 12) {
    pixel = 24 - pixel;
  }
  strip.setPixelColor(pixel + 11, red, green, blue);
  strip.show();
  frame++;
  if(frame > 24) {
    frame = 0;
  }
}

void solid() {
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, red, green, blue);
  }
  strip.show();
}

void pulse() {
  uint8_t scale = frame;
  if(scale > 64) {
    scale = 128 - scale;
  }
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    uint8_t newRed = 4 * map(red, 0, 255, 0, scale);
    uint8_t newGreen = 4 * map(green, 0, 255, 0, scale);
    uint8_t newBlue = 4 * map(blue, 0, 255, 0, scale);
    strip.setPixelColor(i, newRed, newGreen, newBlue);
  }
  strip.show();
  frame = (frame + 1) & 127;
}

void twist() {
  turnOff();
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    if(!(i == frame || ((i + 17) % NUM_PIXELS) == frame)) {
      strip.setPixelColor(i, red, green, blue);
    }
  }
  endBit();
}
