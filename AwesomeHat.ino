#include <string.h>
#include <Arduino.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"


#include <Adafruit_NeoPixel.h>

#define READ_BUFSIZE                    (20)

/* Buffer to hold incoming characters */
uint8_t packetbuffer[READ_BUFSIZE + 1];


Adafruit_NeoPixel strip = Adafruit_NeoPixel(34, 6, NEO_GRB + NEO_KHZ800);
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

int c;
uint8_t red;
uint8_t green;
uint8_t blue;

uint8_t state = 0;

void loop() {
  while ( ble.available() )
  {
//    Serial.println("data available!");
    c = ble.read();

    switch (state) {
      case 0:
        if ( c == '!') {
//          Serial.println("got a !");
          state++;
        }
        break;
      case 1:
        if (c == 'C') {
          state++;
        } else {
          state = 0;
        }
        break;
      case 2: //red
        red = c;
//        Serial.print("red is: "); Serial.print((int)red); Serial.println("");

        state++;
        break;
      case 3: //green;
        green = c;
//        Serial.print("green is: "); Serial.print((int)green); Serial.println("");
        state++;
        break;
      case 4: //blue
        blue = c;
//        Serial.print("blue is: "); Serial.print((int)blue); Serial.println("");
        state = 0;
        break;
    }
  }

  for (uint8_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, red, green, blue);
    strip.show();
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

