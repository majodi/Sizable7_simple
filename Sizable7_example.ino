/*
    Sizable7 example sketch

    https://sizable7.nickstick.nl
    
*/

#include "FastLED.h"                    // FastLED libary should be installed

// change these values when needed
#define DATA_PIN 4                      // GPIO data pin
#define S7_MODULES 1                    // number of Sizable7 modules in chain (this example uses just 1 module)

#define NUM_LEDS S7_MODULES*36          // total number of LEDs

const uint8_t segmentLed[7][5] = {      // mapping of LEDs per segment
  {0,1,2,3,4},
  {5,6,7,8,9},
  {10,11,12,13,14},
  {15,16,17,18,19},
  {20,21,22,23,24},
  {25,26,27,28,29},
  {30,31,32,33,34}
};
const uint8_t digitSegment[10][7] = {   // mapping of segments per digit 0-9
  {0,1,2,3,4,5,99}, //0
  {1,2,99},         //1
  {0,1,6,4,3,99},   //2
  {0,1,6,2,3,99},   //3
  {5,6,1,2,99},     //4
  {0,5,6,2,3,99},   //5
  {0,5,6,4,2,3,99}, //6
  {0,1,2,99},       //7
  {0,1,2,3,4,5,6},  //8
  {0,1,5,6,2,3,99}  //9
};

CRGB leds[S7_MODULES*36];               // FastLED LEDs

//==== setDigit helper function ====

// setDigit will set digit, you should call show() afterwards to actually show the set digit(s)
void setDigit(CRGB leds[], uint8_t module, uint8_t digit, uint8_t h, uint8_t s, uint8_t v) {
  if ((module > S7_MODULES-1) || (digit > 9)) return;
  FastLED.clear(false);
  for(uint8_t segment = 0; segment < 7; segment++) {
    uint8_t onSegment = digitSegment[digit][segment];
    if (onSegment > 6) break;
    for(uint8_t led = 0; led < 5; led++) {
      leds[(segmentLed[onSegment][led])+(module*36)].setHSV(h, s, v);
    }
  }
}

//===== Fire effect animation =====
#define COOLING  75
#define SPARKING 100
#define FIRE_LEDS 15

void fire2012()
{
  for( int s7Module = 0; s7Module < S7_MODULES; s7Module++) {
    static uint8_t heat[S7_MODULES][FIRE_LEDS];
    for( int i = 0; i < FIRE_LEDS; i++) {
      heat[s7Module][i] = qsub8( heat[s7Module][i],  random8(0, ((COOLING * 10) / FIRE_LEDS) + 2));
    }
    for( int k= FIRE_LEDS - 1; k >= 2; k--) {
      heat[s7Module][k] = (heat[s7Module][k - 1] + heat[s7Module][k - 2] + heat[s7Module][k - 2] ) / 3;
    }
    if( random8() < SPARKING ) {
      int y = random8(3);
      heat[s7Module][y] = qadd8( heat[s7Module][y], random8(160,255) );
    }
    for( int j = 0; j < FIRE_LEDS; j++) {
      CRGB color = HeatColor( heat[s7Module][j]);
      int pixelnumber;
      pixelnumber = j;
      pixelnumber = pixelnumber < 10 ? pixelnumber + 20 : pixelnumber - 10; // adjust for S7
      leds[(s7Module * 36) + pixelnumber] = color;
      if (pixelnumber > 24 && pixelnumber < 30) {
        leds[5 + (s7Module * 36) + pixelnumber] = color;
      }
      pixelnumber = (FIRE_LEDS-1) - j;
      pixelnumber = pixelnumber + 5; // adjust for S7
      leds[(((S7_MODULES-1) - s7Module) * 36) + pixelnumber] = color;
    }
  }
}

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,750*S7_MODULES);
  FastLED.clear(true);    
}

void loop() {
  for(uint8_t color = 0; color < 256; color += 32) {              // walk through colors in big steps (main colors)
    for(uint8_t digit = 0; digit < 10; digit++) {                 // walk through digits 0-9
      setDigit(leds, 0, digit, color, 255, 255);                  // set digit/color
      FastLED.show();                                             // show
      delay(200);                                                 // wait a while
    };
    leds[35].setHSV(color, 255, 255);                             // turn on the dot
    FastLED.show();                                               // show it
    delay(400);                                                   // wait a while
    if (color == 224) break;                                      // break on last main color to move to fire animation
  }
  FastLED.clear(true);
  while (true) {                                                  // fire animation forever...
    EVERY_N_MILLISECONDS(65) {
      fire2012();
      FastLED.show();
    }
  }
}
