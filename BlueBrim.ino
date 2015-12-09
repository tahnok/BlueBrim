#include <string.h>
#include <Arduino.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"


#include <Adafruit_NeoPixel.h>

//packet stuff
#define PACKET_ACC_LEN                  (15)
#define PACKET_GYRO_LEN                 (15)
#define PACKET_MAG_LEN                  (15)
#define PACKET_QUAT_LEN                 (19)
#define PACKET_BUTTON_LEN               (5)
#define PACKET_COLOR_LEN                (6)
#define PACKET_LOCATION_LEN             (15)

#define READ_BUFSIZE                    (20)

//mode multi color
#define SPARKLE   (1)
#define RAINBOW   (2)
#define CYCLE     (3)
#define PARTY     (4)
//mode single color
#define CYCLON    (5)
#define TWIST     (6)
#define PULSE     (7)
#define LONELY    (8)

/* Buffer to hold incoming characters */
uint8_t packetbuffer[READ_BUFSIZE + 1];

#define NUM_PIXELS (34)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, 6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel statusLight = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);


Adafruit_BluefruitLE_UART ble(Serial1, -1);


void setup() {
  statusLight.begin();
  statusLight.show();

  //  //DELETE ME AFTER TESTING
  //  while (!Serial);  // required for Flora & Micro
  //  delay(500);
  //  Serial.begin(9600);


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  if ( !ble.begin() )
  {
    error();
  }
}

uint8_t red;
uint8_t green;
uint8_t blue;
uint8_t mode = SPARKLE;

void loop() {
    readPacket(&ble, 200);
  
    if (packetbuffer[1] == 'C') {
      setColor();
    } else if (packetbuffer[1] == 'B' && packetbuffer[3] == '1') {
      setMode();
    }
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
  }
}

void sparkle() {
  for (uint8_t j = 5; j < 16; j++) {
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
      if (!(i % j)) {
        strip.setPixelColor(i, 8 * j, 8 * j, 8 * j);
      }
    }
    strip.show();
    delay(100);
  }
  for (uint8_t j = 16; j > 5; j--) {
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
      if (!(i % j)) {
        strip.setPixelColor(i, 8 * j, 8 * j, 8 * j);
      }
    }
    strip.show();
    delay(100);
  }
}

void cycle() {
  for (uint8_t c; c < 255; c++) {
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
      strip.setPixelColor(i, Wheel(c));
    }
    strip.show();
    delay(50);
  }
}

void rainbow() {
  for (uint8_t j = 0; j < strip.numPixels(); j++) {
    for (uint8_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(map((i + j), 0, strip.numPixels(), 0, 255)));
    }
    strip.show();
    delay(50);
  }
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

uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout)
{
  uint16_t origtimeout = timeout, replyidx = 0;

  memset(packetbuffer, 0, READ_BUFSIZE);

  while (timeout--) {
    if (replyidx >= 20) break;
    if ((packetbuffer[1] == 'A') && (replyidx == PACKET_ACC_LEN))
      break;
    if ((packetbuffer[1] == 'G') && (replyidx == PACKET_GYRO_LEN))
      break;
    if ((packetbuffer[1] == 'M') && (replyidx == PACKET_MAG_LEN))
      break;
    if ((packetbuffer[1] == 'Q') && (replyidx == PACKET_QUAT_LEN))
      break;
    if ((packetbuffer[1] == 'B') && (replyidx == PACKET_BUTTON_LEN))
      break;
    if ((packetbuffer[1] == 'C') && (replyidx == PACKET_COLOR_LEN))
      break;
    if ((packetbuffer[1] == 'L') && (replyidx == PACKET_LOCATION_LEN))
      break;

    while (ble->available()) {
      char c =  ble->read();
      if (c == '!') {
        replyidx = 0;
      }
      packetbuffer[replyidx] = c;
      replyidx++;
      timeout = origtimeout;
    }

    if (timeout == 0) break;
    delay(1);
  }

  packetbuffer[replyidx] = 0;  // null term

  if (!replyidx)  // no data or timeout
    return 0;
  if (packetbuffer[0] != '!')  // doesn't start with '!' packet beginning
    return 0;

  // check checksum!
  uint8_t xsum = 0;
  uint8_t checksum = packetbuffer[replyidx - 1];

  for (uint8_t i = 0; i < replyidx - 1; i++) {
    xsum += packetbuffer[i];
  }
  xsum = ~xsum;

  // Throw an error message if the checksum's don't match
  if (xsum != checksum) {
    return 0;
  }

  // checksum passed!
  return replyidx;
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
