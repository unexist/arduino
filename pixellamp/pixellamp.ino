/**
   @package pixellamp

   @file Pixellamp arduino sketch
   @copyright (c) 2017 Christoph Kappel <unexist@subforge.org>
   @version $Id$

   This program can be distributed under the terms of the GNU GPL.
   See the file COPYING.
 **/

#include <Adafruit_NeoPixel.h>

/* LED stuff */
#define PIXEL_COUNT 37
#define PIXEL_PIN 10
#define PIXEL_TYPE (NEO_RGB + NEO_KHZ800)

/* Button */
#define BUTTON_PIN 3

/* Debug */
//#define DEBUG 1

uint32_t decayTime = 2835;                  // Start dimming light after elapsed seconds
uint32_t decayDelay = 3;                    // Seconds between decay fade-out steps
// Day mode starts at duskHours[0], ends at duskHours[1]
uint16_t maxDayBrightness = 180;            // 0 - 255, lamp will not exceed this during the day
uint16_t maxDuskBrightness = 70;            // 0 - 255, lamp will not exceed this during dusk
uint16_t maxNightBrightness = 20;           // 0 - 255, lamp will not exceed this during the night
float fadeRate = 0.95;                      // Fireworks Variable: 0.01-0.99, controls decay speed

double lastColorUpdate = 0;                 // Epoch of last color update (local or remote)
String colorFromID;                         // String, Tracks who sent the color (for debug)
uint16_t colorRecieved;                     // 0 - 255, Tracks the color received from another lamp
bool lampOn = 0;                            // Tracks if the lamp is lit
uint8_t activeColor = 0;                    // 0 - 255, Tracks what color is currently active (start at red)
uint8_t activeR = 255;                      // 0 - 255, Red component of activeColor;
uint8_t activeG = 0;                        // 0 - 255, Green component of activeColor;
uint8_t activeB = 0;                        // 0 - 255, Blue component of activeColor;
double lastDecayDelay = 0;                  // Time Tracker for decayDelay
uint16_t lampBrightness = 0;                // 0 - 255, Tracks current lamp brightness
uint16_t maxBrightness = maxDayBrightness;  // Assigned the current max brightness
uint8_t dayTrack = 0;                       // Track day/dusk/night condition
uint8_t fadeColor = 0;                      // Track color for special events
byte activePixels = 0;                      // Tracks number of active pixels, 0 is first pixel
float redStates[PIXEL_COUNT];               // Fireworks Variable
float blueStates[PIXEL_COUNT];              // Fireworks Variable
float greenStates[PIXEL_COUNT];             // Fireworks Variable
uint8_t heartbeatDirector = 0;              // Heartbeat Tracking
uint8_t heartbeatColor = 0;                 // Heartbeat Tracking

/* State */
volatile uint8_t buttonState = 0;
volatile uint8_t lampMode = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

void setup() {
  strip.begin();
  strip.show();

  /* Button */
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(0, pin_ISR, CHANGE);

#ifdef DEBUG
  Serial.begin(9600);
#endif /* DEBUG */      
}

void pin_ISR() {
  buttonState = digitalRead(BUTTON_PIN);
#ifdef DEBUG
      Serial.print(buttonState);
      Serial.println();
#endif /* DEBUG */

  if(HIGH == buttonState) {
    if(6 > lampMode) lampMode++;
    else lampMode = 0;
  }
}

void loop() {
#ifdef DEBUG
      Serial.print(lampMode);
      Serial.println();
#endif /* DEBUG */
  
  switch(lampMode) {
    case 0: idleFireworks(0); break;    
    case 1: rainbowFull(30, 1, 0); break;    
    case 2: idleColorFlicker(21); break;
    case 3: idleDisco(); break;
    case 4: idleColorFader(0, 85); break;
    case 5: idleHeartbeat(); break;
    case 6: extinguish(); break;
  }
}

/**
 * Set color
 **/
 
void setColor(byte c) { // c is color.  This function does a "random dither" to set the new color
  // Determine highest bit needed to represent pixel index
  uint32_t color = wheelColor(c, lampBrightness);
  int hiBit = 0;
  int n = strip.numPixels() - 1;
  
  for (unsigned int bit = 1; bit < 0x8000; bit <<= 1) {
    if (n & bit) hiBit = bit;
  }

  int bit, reverse;
  
  for (int i = 0; i < (hiBit << 1); i++) {
    // Reverse the bits in i to create ordered dither:
    reverse = 0;
    for (bit = 1; bit <= hiBit; bit <<= 1) {
      reverse <<= 1;
      if (i & bit) reverse |= 1;
    }
    strip.setPixelColor(reverse, color);
    strip.show();
    delay(20);
  }
  
  activeColor = c;
}

/**
 * Extinguish lamp
 **/

void extinguish() { //Dims the lamp by one unit until lampBrightness is 0 and lampOn is 0
  lampBrightness--;
  uint32_t color = wheelColor(activeColor, lampBrightness);
  
  for (byte j = 0; j <= strip.numPixels(); j++) {
    strip.setPixelColor(j, color);
  }
  
  strip.show();
  
  if (lampBrightness <= 0) {
    lampOn = 0; //If the lamp is completely off, set lampOn to 0
    lampBrightness = 0; // Make sure this number isn't negative somehow
  }
}

/**
 * Select color from color wheel
 **/

uint32_t wheelColor(uint16_t WheelPos, uint16_t iBrightness) {
  float R, G, B;
  float brightness = iBrightness / 255.0;

  if (WheelPos < 85) {
    R = WheelPos * 3;
    G = 255 - WheelPos * 3;
    B = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    R = 255 - WheelPos * 3;
    G = 0;
    B = WheelPos * 3;
  } else {
    WheelPos -= 170;
    R = 0;
    G = WheelPos * 3;
    B = 255 - WheelPos * 3;
  }
  activeR = R * brightness;// + .5;
  activeG = G * brightness;// + .5;
  activeB = B * brightness;// + .5;
  
  return strip.Color((byte) activeR, (byte) activeG, (byte) activeB);
}

/**
 * Ranbow colors
 **/

void rainbowFull(byte wait, byte fade, byte pendulum) {
  uint8_t lastMode = lampMode;
  uint16_t i, j, k;
  byte dir = 1;
  if (fade == 0) k = 0;
  else k = maxBrightness;

  for (j = 0; j <= 255 && lastMode == lampMode; j++) {
    for (i = 0; i < strip.numPixels() && lastMode == lampMode; i++) {
      strip.setPixelColor((strip.numPixels() - 1) - i, 
        wheelColor(((i * 60 / strip.numPixels()) + j) & 255, k));
    }

    strip.show();
    delay(wait);

    if ((1 == pendulum && 1 == dir) || (fade == 0 && k < maxBrightness)) {
      k++;
      if (k == maxBrightness) dir = 2;
    } else if ((1 == pendulum && 2 == dir) || (fade == 2 && k > 0)) {
      if (1 == k) dir = 1;
      k--;
    }
  }
}

/**
 * Simple color fader
 **/

void idleColorFader(uint8_t c1, uint8_t c2) {
  uint8_t lastMode = lampMode;
  lampBrightness = 40;
  
  if ( maxBrightness < lampBrightness ) {
    lampBrightness = maxBrightness;
  }
  
  uint16_t currR, currG, currB, endR, endG, endB;
  uint32_t color = wheelColor(fadeColor, lampBrightness);
  endR = (uint16_t)((color >> 16) & 0xff); // Splits out new color into separate R, G, B
  endG = (uint16_t)((color >> 8) & 0xff);
  endB = (uint16_t)(color & 0xff);
  
  for (uint16_t j = 0; j < activePixels && lastMode == lampMode; j++) {
    long startRGB = strip.getPixelColor(j); // Get pixel's current color
    currR = (uint16_t)((startRGB >> 16) & 0xff); // Splits out current color into separate R, G, B
    currG = (uint16_t)((startRGB >> 8) & 0xff);
    currB = (uint16_t)(startRGB & 0xff);
    
    if ( currR > endR ) {
      currR = currR - 1;
    } else if ( currR < endR ) {
      currR = currR + 1;
    } else {
      currR = endR;
    }
    if ( currG > endG ) {
      currG = currG - 1;
    } else if ( currG < endG ) {
      currG = currG + 1;
    } else {
      currG = endG;
    }
    if ( currB > endB ) {
      currB = currB - 1;
    } else if ( currB < endB ) {
      currB = currB + 1;
    } else {
      currB = endB;
    }

    //Catch overflows
    currR %= 255;
    currG %= 255;
    currB %= 255;

    strip.setPixelColor(j, currR, currG, currB);
    if ( j >= strip.numPixels() - 1 && endR == currR && endG == currG && endB == currB) {
      if ( fadeColor == c1 ) {
        fadeColor = c2;
      } else {
        fadeColor = c1;
      }
      activePixels = 0;
    }
  }
  strip.show();
  if ( activePixels < strip.numPixels() ) activePixels++;
  delay(20);
}

/**
 * Simple fireworks
 **/

void idleFireworks(uint8_t w) {
  uint8_t lastMode = lampMode;

  // w = 0 for mulitcolor, w = 1 for all white flashes
  lampBrightness = 40;
  
  if ( maxBrightness < lampBrightness ) {
    lampBrightness = maxBrightness;
  }
  
  if (random(20) == 1) {
    uint16_t i = random(strip.numPixels());
    if (redStates[i] < 1 && greenStates[i] < 1 && blueStates[i] < 1) {
      if (w == 0) {
        redStates[i] = random(lampBrightness);
        greenStates[i] = random(lampBrightness);
        blueStates[i] = random(lampBrightness);
      } else {
        redStates[i] = lampBrightness;
        greenStates[i] = lampBrightness;
        blueStates[i] = lampBrightness;
      }
    }
  }
  
  for (uint16_t l = 0; l < strip.numPixels() && lastMode == lampMode; l++) {
    if (redStates[l] > 1 || greenStates[l] > 1 || blueStates[l] > 1) {
      strip.setPixelColor(l, redStates[l], greenStates[l], blueStates[l]);
      if (redStates[l] > 1) {
        redStates[l] = redStates[l] * fadeRate;
      } else {
        redStates[l] = 0;
      }

      if (greenStates[l] > 1) {
        greenStates[l] = greenStates[l] * fadeRate;
      } else {
        greenStates[l] = 0;
      }

      if (blueStates[l] > 1) {
        blueStates[l] = blueStates[l] * fadeRate;
      } else {
        blueStates[l] = 0;
      }

    } else {
      strip.setPixelColor(l, 0, 0, 0);
    }
  }
  strip.show();
}

/**
 * Simple disco lights
 **/

void idleDisco() {
  uint8_t lastMode = lampMode;
  lampBrightness = 40;
  
  if ( maxBrightness < lampBrightness ) {
    lampBrightness = maxBrightness;
  }
  
  for (unsigned int i = 0; i < strip.numPixels() && lastMode == lampMode; i++) {
    int randr = random(0, lampBrightness);
    int randg = random(0, lampBrightness);
    int randb = random(0, lampBrightness);
    int randi = random(0, (strip.numPixels() - 1));
    
    strip.setPixelColor(randi, randr, randg, randb);
    strip.show();
    
    delay(5);
  }
}

/**
 * Simple color flicker
 **/

void idleColorFlicker(uint8_t c) {
  uint8_t lastMode = lampMode;
  lampBrightness = 20;
  
  if ( maxBrightness < lampBrightness ) {
    lampBrightness = maxBrightness;
  }
  
  uint32_t color = wheelColor(c, lampBrightness);

  for (uint8_t i = 0; i < strip.numPixels() && lastMode == lampMode; i++) {
    uint8_t flicker = random(0, 10);
    
    int flickerR = (uint16_t)((color >> 16) & 0xff) - flicker; // Splits out new color into separate R, G, B
    int flickerG = (uint16_t)((color >> 8) & 0xff) - flicker;
    int flickerB = (uint16_t)(color & 0xff) - flicker;
    
    if (flickerR < 0) flickerR = 0;
    if (flickerG < 0) flickerG = 0;
    if (flickerB < 0) flickerB = 0;
    
    strip.setPixelColor(i, flickerR, flickerG, flickerB);
  }
  
  strip.show();
  delay(30);
}

/**
 * Simple heartbeat
 **/

void idleHeartbeat() {
  lampBrightness = 20;
  if ( maxBrightness < lampBrightness ) {
    lampBrightness = maxBrightness;
  }
  uint8_t endColor = 0;

  if ( heartbeatDirector == 0 ) {
    endColor = lampBrightness * 0.6;
  } else if ( heartbeatDirector == 1 ) {
    endColor = lampBrightness * 0.2;
  } else if ( heartbeatDirector == 2 ) {
    endColor = lampBrightness;
  } else if ( heartbeatDirector == 3 ) {
    endColor = lampBrightness * 0.12;
  } else {
    //do nothing, this will delay
  }

  if ( heartbeatColor < endColor ) {
    for (int j = heartbeatColor; j < endColor; j += 4) {
      for (int i = 25; i < 35; i++) {
        strip.setPixelColor(i, j, 0, 0);

      }
      strip.show();
      delay(15);
    }
  } else if ( heartbeatColor > endColor ) {
    for (int j = heartbeatColor; j > endColor; j--) {
      for (int i = 25; i < 35; i++) {
        strip.setPixelColor(i, j, 0, 0);

      }
      strip.show();
      delay(30);
    }
  } else {
    delay(15);
    delay(15);
    delay(15);
    delay(15);
    delay(15);
    delay(15);
    delay(15);
  }

  heartbeatColor = endColor;

  heartbeatDirector++;
  heartbeatDirector %= 4;
}

