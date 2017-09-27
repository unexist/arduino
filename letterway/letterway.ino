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

/* Structs */
typedef struct letter_t {
  char letter;
  short masks[5];
} Letter;

typedef struct color_t {
  int r;
  int g;
  int b;
} Col;

typedef struct row_t {
  int   mask;
  row_t *next;
} Row;

Letter letters[] = {
  { 'a', { 
      B00011000,
      B00100100,
      B01000010,
      B01111110,
      B10000001
    }
  },
  { 'b', { 
      B11111110,
      B10000001,
      B11111110,
      B10000001,
      B11111110
    }
  },
  { 'c', { 
      B01111110,
      B10000001,
      B10000000,
      B10000001,
      B01111110
    }
  },
  { 'd', { 
      B11111110,
      B10000001,
      B10000001,
      B10000001,
      B11111110
    }
  },
  { 'e', { 
      B11111111,
      B10000000,
      B11111100,
      B10000000,
      B11111111
    }
  },
  { 'f', { 
      B11111111,
      B10000000,
      B11111100,
      B10000000,
      B10000000
    }
  },
  { 'g', { 
      B01111111,
      B10000000,
      B10001110,
      B10000001,
      B01111110
    }
  },
  { 'h', { 
      B10000001,
      B10000001,
      B11111111,
      B10000001,
      B10000001
    }
  },
  { 'i', { 
      B00111000,
      B00010000,
      B00010000,
      B00010000,
      B00111000
    }
  },
  { 'j', { 
      B11111111,
      B00000001,
      B01000001,
      B10000001,
      B01111110
    }
  },
  { 'k', { 
      B10001000,
      B10010000,
      B11100000,
      B10010000,
      B10001000
    }
  },
  { 'l', { 
      B10000000,
      B10000000,
      B10000000,
      B10000000,
      B11111111
    }
  },
  { 'm', { 
      B11000011,
      B10100101,
      B10011001,
      B10000001,
      B10000001
    }
  },
  { 'n', { 
      B11000010,
      B10100010,
      B10010010,
      B10001010,
      B10000110
    }
  },
  { 'o', { 
      B01111110,
      B10000001,
      B10000001,
      B10000001,
      B01111110
    }
  },
  { 'p', { 
      B11111110,
      B10000001,
      B11111110,
      B10000000,
      B10000000
    }
  },
  { 'q', { 
      B01111110,
      B10000001,
      B10100001,
      B11000001,
      B11111110
    }
  },
  { 'r', { 
      B11111000,
      B10000100,
      B11111000,
      B10010000,
      B10011000
    }
  },
  { 's', { 
      B01111110,
      B10000001,
      B00100000,
      B10000100,
      B01111110
    }
  },
  { 't', { 
      B11111110,
      B00010000,
      B00010000,
      B00010000,
      B00010000
    }
  },
  { 'u', { 
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000
    }
  },
  { 'v', { 
      B10000001,
      B10000001,
      B01000010,
      B00100100,
      B00011000
    }
  },
  { 'w', { 
      B10000001,
      B10000001,
      B01011010,
      B01011010,
      B00100100
    }
  },
  { 'x', { 
      B10000100,
      B01001000,
      B00110000,
      B01001000,
      B10000100
    }
  },
  { 'y', { 
      B10000010,
      B01000100,
      B00101000,
      B00010000,
      B00010000
    }
  },
  { 'z', { 
      B1111100,
      B0001000,
      B0010000,
      B0100000,
      B1111100
    }
  }
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

void setup() {
  strip.begin();
  strip.show();
}

void
setForRow(int gid,
  Col *c)
{
  int offset = (gid * PIXEL_PER_ROW);
  
  for(int pos = offset; pos < offset + PIXEL_PER_ROW; pos++)
    {
      strip.setPixelColor(pos, c->r, c->g, c->b);
    }

  strip.show();
}

void
rgbSteps()
{
  int idx = 0;
  int dir = 1;
  Col c = { 255, 0, 0 };

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
drawRows(Row *row,
  Col *fg,
  Col *bg)
{
  int offset = 0;
  Col *curcol;
  Row *currow = row;

  for(;;)
   {
      for(int i = 0; i < NROWS; i++)
        {
          int pos = i * PIXEL_PER_ROW;

          if(0 == offset)
            {
              for(int j = 0; j < 8; j++)
                {
                  curcol = ((currow->mask & (1 << j)) ? fg : bg);
              
                  strip.setPixelColor(pos + j, curcol->r,
                    curcol->g, curcol->b);
                }

              offset = 1;
            }
          else
            {
              for(int j = 8; j > 0; j--)
                {
                  curcol = ((currow->mask & (1 << j)) ? fg : bg);
              
                  strip.setPixelColor(pos + j + PIXEL_PER_ROW - 8, curcol->r,
                    curcol->g, curcol->b);
                }  

                offset = 0;
            }

          currow = currow->next;
        }

      strip.show();
      delay(600);
   }
}

void loop()
{
  rgbSteps();
}

void loop1() {
  char text[] = { 'k', 'n', 'u', 'b', 's' };
  
  Col fg = { 255,   0,   0 };
  Col bg = {   0,   0, 255 };

  Row *rows = NULL;
  Row *head = NULL;

  /* Create rows */
  for(int i = 0; i < sizeof(text); i++)
    {
      Row    *r = NULL;
      Letter *l = &letters[97 - text[i]];

      for(int j = 0; j < 6; j++)
        {
          r = (Row *)calloc(1, sizeof(Row));

          r->mask = (j < 5 ? l->masks[j] : 0); ///< Add empty row

          if(NULL != rows) 
            {
              rows->next = r;
              rows = r;
            }
          else
            {
              rows = r;
              head = r;
            }
        }
    }

  rows->next = head; ///< Forever and ever

  drawRows(head, &fg, &bg);
}

