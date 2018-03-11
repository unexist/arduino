/**
   @package tvbank

   @file TVbank arduino sketch
   @copyright (c) 2018 Christoph Kappel <unexist@subforge.org>
   @version $Id$

   This program can be distributed under the terms of the GNU GPL.
   See the file COPYING.
 **/

#include <Adafruit_NeoPixel.h>
#include <IRremote.h>
#include <IRremoteInt.h>

/* Config and pins */
#define PIXEL_COUNT 84
#define PIXEL_PER_ROW (PIXEL_COUNT / 2)
#define PIXEL_PER_COL (PIXEL_PER_ROW / 3)
#define PIXEL_TYPE NEO_RGB + NEO_KHZ800
#define PIXEL_PIN 8
#define IRDA_PIN 7

/* IRda keys */
#define KEY_1     0xE318261B
#define KEY_2     0xFF629D
#define KEY_3     0xFFE21D
#define KEY_4     0xFF22DD
#define KEY_5     0xD7E84B1B
#define KEY_6     0x20FE4DBB
#define KEY_7     0xFFE01F
#define KEY_8     0xA3C8EDDB
#define KEY_9     0xE5CFBD7F
#define KEY_UP    0xFF18E7
#define KEY_DOWN  0x1BC0157B
#define KEY_LEFT  0x8C22657B
#define KEY_RIGHT 0xFF5AA5
#define KEY_RIGHT 0x449E79F

/* LEd strip */
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

/* IRda */
IRrecv receiver(IRDA_PIN);
decode_results results;

/* Globals */
int bright = 255;

/**
 * Set up
 **/
void setup() {
  /* Init strip */
  strip.begin();
  strip.show();

  /* Init IRda */
  receiver.enableIRIn();

  /* Init serial */
  Serial.begin(9600);
}

void setRowColColor(int drow, int dcol, int dr, int dg, int db) {
  int pos = (drow * PIXEL_PER_ROW + dcol * PIXEL_PER_COL);
  
  for(int i = pos; i < pos + PIXEL_PER_COL; i++) {
    strip.setPixelColor(pos, dr, dg, db);
  }

  strip.show();
}

/**
 * Just loop
 **/
void loop() {
  if(receiver.decode(&results)) {
    int row = 0;
    int col = 0;
    int r = 0;
    int g = 0;
    int b = 0;

    /* Handle values */
    switch(results.value) {
      case KEY_1: row = 0; col = 0; setRowColColor(row, col, r, g, b); break; /// Row 1, Col 1
      case KEY_2: row = 0; col = 1; setRowColColor(row, col, r, g, b); break; /// Row 1, Col 2
      case KEY_3: row = 0; col = 2; setRowColColor(row, col, r, g, b); break; /// Row 1, Col 3
      
      case KEY_4: row = 1; col = 0; setRowColColor(row, col, r, g, b); break; /// Row 2, Col 1
      case KEY_5: row = 1; col = 1; setRowColColor(row, col, r, g, b); break; /// Row 2, Col 2
      case KEY_6: row = 1; col = 2; setRowColColor(row, col, r, g, b); break; /// Row 2, Col 3

      case KEY_7: r = bright; g =      0; b =      0; setRowColColor(row, col, r, g, b); break; /// Red 
      case KEY_8: r =      0; g = bright; b =      0; setRowColColor(row, col, r, g, b); break; /// Green
      case KEY_9: r =      0; g =      0; b = bright; setRowColColor(row, col, r, g, b); break; /// Blue

      case KEY_UP:   if(255 > bright) bright += 5; setRowColColor(row, col, r, g, b); break; /// Increase brightness
      case KEY_DOWN: if(0 < bright)   bright -= 5; setRowColColor(row, col, r, g, b); break; /// Decrease brightness

      case KEY_LEFT:  break; /// Select next color
      case KEY_RIGHT: break; /// Select prev color
    }

    
    
    Serial.println(results.value, HEX);
    Serial.println(row);
    Serial.println(col);

    receiver.resume();
  }
}
