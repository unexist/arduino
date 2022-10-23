/**
 * @package matrixtable
 *
 * @file Matrixtable arduino sketch
 * @copyright (c) 2022 Christoph Kappel <christoph@unexist.dev>
 * @version $Id$
 *
 * This program can be distributed under the terms of the GNU GPL.
 * See the file COPYING.
 **/

#include <Adafruit_NeoPixel.h>

/* LED stuff */
#define PIXEL_COUNT 224
#define PIXEL_PIN 2
#define PIXEL_TYPE (NEO_GRB + NEO_KHZ800)

/* Button */
#define BUTTON_PIN 3

/* Debug */
//#define DEBUG 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

void
setPixel(uint8_t x,
  uint8_t r,
  uint8_t g,
  uint8_t b)
{
  strip.setPixelColor(x, r, g, b);
}

/**
 * Set up arduino
 **/
 
void
setup()
{
  strip.begin();
  strip.show();

#ifdef DEBUG
  Serial.begin(9600);
#endif /* DEBUG */

  for(uint8_t i = 0; i < PIXEL_COUNT; i++)
    setPixel(i, 255, 0, 0);
    
  strip.show();
}

/**
 * Run in loop
 **/

void
loop()
{
  /* We do nothing here */
}
