/**
 * @package matrixtable
 *
 * @file Matrixtable arduino sketch
 * @copyright (c) 2018 Christoph Kappel <unexist@subforge.org>
 * @version $Id$
 *
 * This program can be distributed under the terms of the GNU GPL.
 * See the file COPYING.
 **/

#include <Adafruit_NeoPixel.h>

/* LED stuff */
#define PIXEL_COUNT 192
#define PIXEL_PIN 2
#define PIXEL_TYPE (NEO_GRB + NEO_KHZ800)

#define PIXEL_PER_ROW 14
#define PIXEL_PER_SMALL_ROW 12
#define PIXEL_DIFF (PIXEL_PER_ROW - PIXEL_PER_SMALL_ROW)
#define PIXEL_ROWS 14

/* Button */
#define BUTTON_PIN 3

/* Debug */
//#define DEBUG 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

typedef struct image_t {
  uint8_t data[PIXEL_ROWS][PIXEL_PER_ROW];
  uint8_t palette[9][3];
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
    { 255, 0, 255 }
  }
};

void
setPixel(uint8_t x,
  uint8_t y,
  uint8_t r,
  uint8_t g,
  uint8_t b)
{
  uint8_t pixel = 0;

  /* Skip corners */
  if((0 == x && 0 == y) || (PIXEL_PER_ROW - 1 == x && 0 == y) ||
      (0 == x && PIXEL_ROWS - 1 == y) ||
      (PIXEL_PER_ROW - 1 == x && y == PIXEL_ROWS - 1))
    return;

  /* Flip x */
  if(0 == y % 2)
    {
      pixel = x;
    }
  else
    {
      pixel = PIXEL_PER_ROW - x;
    }

  /* Add y */
  pixel += y * PIXEL_PER_ROW;

  /* Fix corners */
  if(0 < y)                 pixel -= PIXEL_DIFF;
  if((PIXEL_ROWS - 1) == y) pixel -= PIXEL_DIFF;

  strip.setPixelColor(pixel, r, g, b);
}

void
drawImage(Image *img)
{
  for (int x = 0; x < PIXEL_PER_ROW; x++)
    {
      for (int y = 0; y < PIXEL_ROWS; y++)
        {
          uint8_t *pal = img->palette[(uint8_t)img->data[x][y]];
          
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

  drawImage(&quack);
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
