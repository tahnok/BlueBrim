#include <string.h>
#include <Arduino.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"
#include <Adafruit_NeoPixel.h>

//mode multi color
#define SPARKLE   (1)
#define RAINBOW   (2)
#define CYCLE     (3)
#define PARTY     (4)
//mode single color
#define CYLON     (5)
#define TWIST     (6)
#define PULSE     (7)
#define SOLID     (8)

#define NUM_PIXELS (34)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, 6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel statusLight = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);

Adafruit_BluefruitLE_UART ble(Serial1, -1);

//see bluetooth.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
extern uint8_t packetbuffer[];

void setup() {
  statusLight.begin();
  statusLight.show();

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  if (!ble.begin()) {
    error();
  }
  randomSeed(analogRead(0));
}

uint8_t red = 100;
uint8_t green;
uint8_t blue;
uint8_t mode = CYLON;

uint32_t frame = 0;

void loop() {
  if (ble.available()) {
    readPacket(&ble, 1000);
    frame = 0;
    if (packetbuffer[1] == 'C') {
      setColor();
      mode = SOLID;
    } else if (packetbuffer[1] == 'B' && packetbuffer[3] == '1') {
      setMode();
    }
  }
  //animation frame
  switch (mode) {
    case SPARKLE:
      sparkle();
      break;
    case RAINBOW:
      rainbow();
      break;
    case CYCLE:
      cycle();
      break;
    case PARTY:
      party();
      break;
    case CYLON:
      cylon();
      break;
  }
  delay(50);
}

uint8_t dir = 1;
void sparkle() {
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, frame, frame, frame);
  }
  strip.show();
  if(frame < 0) {
    dir = 1;
  } else if(frame > 100) {
    dir = -1;
  }
  frame = frame + dir;
}

//12 to 24
void cylon() {
  turnOff();
  strip.setPixelColor(frame + 12, red, green, blue);
  strip.show();
  if(frame > 12) {
    dir = -1;
  }
  if(frame <= 0) {
    dir = 1;
  }

  frame = frame + dir;
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
  strip.show();
  frame++;
  if (frame > NUM_PIXELS) {
    frame = 0;
  }
}

void party() {
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, Wheel(random(255)));
  }
  strip.show();
}

void error() {
  for (;;) {
    statusLight.setPixelColor(0, strip.Color(100, 0, 0));
    statusLight.show();
    delay(1000);
    statusLight.setPixelColor(0, 0);
    statusLight.show();
    delay(1000);
  }
}

void setColor() {
  red = packetbuffer[2];
  green = packetbuffer[3];
  blue = packetbuffer[4];
  for (uint8_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, red, green, blue);
    strip.show();
  }
}

void setMode() {
  mode = packetbuffer[2] - '0';
  statusLight.setPixelColor(0, Wheel(mode * 32));
  statusLight.show();
}

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

void turnOff() {
  for(uint8_t i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, 0);
  }
}
