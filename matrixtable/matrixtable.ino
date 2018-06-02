/**
   @package matrixtable

   @file Matrixtable arduino sketch
   @copyright (c) 2017 Christoph Kappel <unexist@subforge.org>
   @version $Id$

   This program can be distributed under the terms of the GNU GPL.
   See the file COPYING.
 **/

#include <Adafruit_NeoPixel.h>

/* LED stuff */
#define PIXEL_COUNT 192
#define PIXEL_PIN 2
#define PIXEL_TYPE (NEO_GRB + NEO_KHZ800)

#define LED_PER_ROW 14

/* Button */
#define BUTTON_PIN 3

/* Debug */
//#define DEBUG 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

typedef struct image_t {
  char data[14][14];
  int palette[9][3];
} Image;

Image quack = {
  {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3, 4, 0, 0, },
    { 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
    { 0, 0, 1, 2, 1, 1, 2, 1, 1, 1, 1, 0, 0, 0, },
    { 0, 0, 1, 1, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, },
    { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, },
    { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 3, 3, 4, 3, 4, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
  },
  {
    {   0,   0,   0 },
    { 255, 193,  60 },
    {  99,  96,  91 },
    { 190,  46,  55 },
    { 255, 125,  63 }
  }
};

Image turtle = {
  {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 1, 1, 0, 0, },
    { 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 1, 0, 1, 0, },
    { 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 1, 1, 1, 0, },
    { 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, },
    { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
  },
  {
    {   0,   0,   0 },
    { 177, 191,  75 },
    {   9, 106,  63 }
  }
};

Image test = {
  {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
  },
  {
    {   0, 0, 255 }
  }
};

void
setPixel(int x,
  int y,
  int r,
  int g,
  int b)
{
  uint8_t pixel = 0;
  
  if (0 == y) {
    pixel = 12 - x;
  } else if (0 == (y % 2)) {
    pixel = 12 + (y - 1) * LED_PER_ROW + x;
  } else {
    pixel = 12 + (y - 1) * LED_PER_ROW + LED_PER_ROW - x;
  }

  strip.setPixelColor(pixel, r, g, b);
}

void
drawImage(Image *img) {
    for (int x = 0; x < 14; x++) {
    for (int y = 0; y < 14; y++) {
      int *pal = img->palette[(int)img->data[x][y]];
      
      setPixel(x, y, pal[0], pal[1], pal[2]);
    }
  }
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

  drawImage(&turtle);

  strip.show();
}

/**
 * Run in loop
 **/

void
loop()
{
}
