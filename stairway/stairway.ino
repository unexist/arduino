/**
   @package stairway

   @file Stairway arduino sketch
   @copyright (c) 2017 Christoph Kappel <unexist@subforge.org>
   @version $Id$

   This program can be distributed under the terms of the GNU GPL.
   See the file COPYING.
 **/

#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

#define NROWS          18                                         ///< Total LED ROWS
#define PIXEL_PER_ROW  23                                         ///< LEDs per group

#define PIXEL_COUNT (NROWS * PIXEL_PER_ROW)
#define PIXEL_PIN 6
#define PIXEL_TYPE (NEO_GRB + NEO_KHZ800)

#define BLACK_PIXEL   0,   0,   0
#define WHITE_PIXEL 255, 255, 255

/* Structs */
typedef struct color_t
{
  int r;
  int g;
  int b;
} Color;

typedef struct roller_t
{
  uint32_t mask;
  struct roller_t *next;
} Roller;

typedef struct figure_t
{
  uint32_t *masks;
  uint8_t nmasks;
} Figure;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

void
setForRow(int16_t rowid,
  Color *c)
{
  int16_t offset = (rowid * PIXEL_PER_ROW);
  
  for(int16_t pos = offset; pos < offset + PIXEL_PER_ROW; pos++)
    {
      strip.setPixelColor(pos, c->r, c->g, c->b);
    }

  strip.show();
}

void
setForCol(int16_t colid,
  Color *c)
{  
  boolean even = true;
  
  for(int16_t row = 0; row < NROWS; row++, even = !even)
    {
      int16_t pos = (row * PIXEL_PER_ROW);
            
      strip.setPixelColor(pos + (even ? colid : (PIXEL_PER_ROW - colid)), 
        c->r, c->g, c->b);
    }

  strip.show();
}

void
setRowMask(int16_t rowid,
  int32_t mask,
  Color *c,
  boolean redraw)
{
  if(0 > rowid) return;
 
  boolean even   = !!(rowid & 1);
  int16_t offset = (rowid * PIXEL_PER_ROW);
  
  for(int16_t b = PIXEL_PER_ROW; b >= 0; b--)
    {
      int16_t pos = (even ? (offset + PIXEL_PER_ROW - b) : offset + b);
      
      if(mask & (1L << b)) strip.setPixelColor(pos, c->r, c->g, c->b);
      else strip.setPixelColor(pos, 0, 0, 0);
    }
 
  if(redraw) strip.show();
}

void
snakeSteps()
{
  boolean reverse = false;
  int16_t mask = 1;
  Color c = { 255, 255, 255 };

  for(int16_t i = 0; i < 16; i++)
    {
      for(int16_t row = 0; row < NROWS; row++)
        {
          setRowMask(row, mask, &c, true);

          if(reverse) mask >>= 2;
          else        mask <<= 2;

          if(mask <= (1L << 0) || mask >= (1L << PIXEL_PER_ROW / 2)) reverse = !reverse;
        }
      mask <<= 1;
    }
}

void
rgbSteps()
{
  int idx = 0;
  int dir = 1;
  Color c = { 255, 0, 0 };

  for(;;)
    {
      setForRow(idx, &c);
      idx += dir;
  
      delay(60);

      if(idx > NROWS || idx <= 0)
        {
          dir = dir * -1;
    
          if(255 == c.r)      { c.r =   0; c.g = 255; c.b =   0; }
          else if(255 == c.g) { c.r =   0; c.g =   0; c.b = 255; }
          else if(255 == c.b) { c.r = 255; c.g =   0; c.b =   0; }
        }
    }
}

void
RollerAddFigure(Roller **r,
  Figure *fig)
{
  for(uint8_t i = 0; i < fig->nmasks; i++)
    {
      Roller *fr = (Roller *)calloc(1, sizeof(Roller));

      fr->mask   = fig->masks[i];
      (*r)->next = fr;
      *r         = fr;
    }  
}

Figure*
FigureCreate(uint8_t nmasks)
{
  Figure *fig = (Figure *)calloc(1, sizeof(Figure));

  fig->masks  = (uint32_t *)calloc(4, sizeof(uint32_t));
  fig->nmasks = nmasks;

  return fig;
}

void
FigureSetMask(Figure *fig,
  uint8_t idx,
  uint32_t mask)
{
  if(idx < fig->nmasks)
    fig->masks[idx] = mask;
}

Roller *head = NULL, *cur = NULL;

void 
setup()
{
  strip.begin();
  strip.show();

  /* Create figures */
  Figure *fig_blank = FigureCreate(1);

  FigureSetMask(fig_blank, 0, 0);
  
  Figure *fig_arrow = FigureCreate(3);
  
  FigureSetMask(fig_arrow, 0, ((1L <<  8)|(1L << 16)));
  FigureSetMask(fig_arrow, 1, ((1L << 10)|(1L << 14)));
  FigureSetMask(fig_arrow, 2, (1L << 12));

  /* Create roller */
  head = cur = (Roller *)calloc(1, sizeof(Roller));

  RollerAddFigure(&cur, fig_blank);
  RollerAddFigure(&cur, fig_arrow);

  cur->next = head;
  cur       = head;
}

void
loop()
{
  Color c = { 255, 0, 0 };

  while(1)
    {
      for(uint8_t i = 0; i < NROWS; i++)
        { 
          setRowMask(i, cur->mask, &c, false);

          cur = cur->next;
        }
        
      strip.show();
      delay(500);
    }
}
