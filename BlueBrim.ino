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

//see colors.cpp
extern void sparkle();
extern void rainbow();
extern void cycle();
extern void party();
extern void cylon();
extern void twist();
extern void pulse();
extern void solid();

void setup() {
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
uint8_t mode = RAINBOW;
uint32_t frame = 0;

void loop() {
  if (ble.available()) {
    readPacket(&ble, 1000);
    frame = 0;
    if (packetbuffer[1] == 'C') {
      setColor();
      if(mode < CYLON) {
        mode = SOLID;
      }
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
    case PULSE:
      pulse();
      break;
    case SOLID:
      solid();
      break;
    case TWIST:
      twist();
      break;
  }
  delay(50);
}

void error() {
  statusLight.begin();
  statusLight.show();
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
}

void setMode() {
  mode = packetbuffer[2] - '0';
}
