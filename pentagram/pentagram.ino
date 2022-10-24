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
#define PIXEL_COUNT 206
#define PIXEL_PIN 2
#define PIXEL_TYPE (NEO_GRB + NEO_KHZ800)

/* Button */
#define BUTTON_PIN 3

/* Debug */
//#define DEBUG 1

#define LENGTH(a) (sizeof(a) / sizeof(a[0])) 

/* Definitions */
typedef struct line_t {
  uint8_t start;
  uint8_t end;
} Line;

typedef struct triangle_t {
  Line l1;
  Line l2;
  Line l3;
} Triangle;

/* Data */
Triangle right_bottom = {
  .l1 = { .start =   1, .end =  14 },
  .l2 = { .start =  99, .end = 108 },
  .l3 = { .start = 192, .end = 206 }
};

Triangle right = {
  .l1 = { .start =  65, .end =  82 },
  .l2 = { .start =  82, .end =  99 },
  .l3 = { .start = 181, .end = 192 }
};

Triangle top = {
  .l1 = { .start =  57, .end =  65 },
  .l2 = { .start = 151, .end = 166 },
  .l3 = { .start = 166, .end = 181 }
};

Triangle left = {
  .l1 = { .start =  25, .end =  40 },
  .l2 = { .start =  40, .end =  56 },
  .l3 = { .start = 141, .end = 151 }
};

Triangle left_bottom = {
  .l1 = { .start =  14, .end =  25 },
  .l2 = { .start = 108, .end = 124 },
  .l3 = { .start = 124, .end = 141 }
};

Triangle *triangles[] = { &top, &right, &right_bottom, &left_bottom, &left };
Line *lines[] = { &(right_bottom.l1), &(left_bottom.l2), &(left_bottom.l3), &(left.l1), &(left.l2), &(top.l2), &(top.l3), &(right.l1), &(right.l2), &(right_bottom.l3) };

uint8_t high[] = { 
  40, 41, 255,
  39, 42, 255,
  38, 43, 255,
  37, 44, 255,
  36, 45, 255,
  35, 46, 255,
  34, 47, 255,
  33, 48, 255,
  32, 39, 255,
  31, 50, 255,
  30, 51, 255,
  29, 52, 255,
  28, 53, 255,
  27, 54, 255,
  26, 55, 255,
  125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 154, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 255,
  25, 57, 125, 167, 255,
  25, 57, 124, 168, 255,
  24, 58, 124, 169, 255,    
  24, 58, 124, 170, 255,
  23, 24, 59, 123, 171, 255,
  23, 59, 60, 123, 171, 255,
  22, 23, 60, 122, 123, 173, 255,
  22, 60, 61, 122, 174, 255,
  22, 61, 122, 175, 255,
  21, 22, 61, 62, 121, 176, 255,
  21, 62, 63, 120, 121, 177, 255,
  20, 21, 63, 119, 20, 178, 255,
  20, 63, 64, 119, 179, 255,
  19, 20, 64, 65, 118,180, 255,
  19, 65, 117, 118, 180, 181, 255,
  18, 19, 66, 116, 117, 182, 255,
  18, 66, 67, 115, 116, 183, 255,
  17, 67, 68, 114, 115, 184, 255,
  16, 17, 68, 69, 112, 113, 114, 185, 255,
  16, 69, 70, 111, 112, 113, 186, 255,
  15, 16, 70, 71, 110, 111, 187, 255,
  15, 71, 72, 109, 110, 111, 188, 255,
  14, 72, 73, 107, 108, 109, 188, 189, 255,
  14, 73, 74, 104, 105, 106, 107, 108, 190, 255,
  13, 74, 75, 102, 103, 104, 105, 191, 255,
  13, 75, 76, 99, 100, 101, 102, 103, 192, 255,
  12, 77, 78, 95, 96, 97, 98, 99, 192, 255,
  11, 12, 78, 79, 94, 96, 97, 98, 193, 255,
  11, 79, 80, 91, 92, 93, 94, 95, 194, 255,
  10, 80, 81, 86, 87, 88, 89, 90, 91, 195, 255,
  9, 10, 82, 83, 84, 85, 86, 87, 196, 255,
  9, 197, 255,
  8, 198, 255,
  7, 199, 255,
  6, 200, 255,
  5, 201, 255,
  4, 202, 255,
  3, 203, 255,
  3, 204, 255,
  2, 205, 255,
  1, 206, 255
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

/* Helper */

void setPixel(uint8_t x, uint8_t r, uint8_t g, uint8_t b) {
  strip.setPixelColor(x, r, g, b);
}

void setAllPixel(uint8_t r, uint8_t g, uint8_t b) {
  for(uint8_t i = 0; i < PIXEL_COUNT; i++) {
    strip.setPixelColor(i, r, g, b);
  }    
}

void drawLine(Line *l, uint8_t r, uint8_t g, uint8_t b) {   
  if (NULL != l) { 
    for (uint8_t i = l->start; i < l->end; i++)
      setPixel(i, r, g, b);
  }      
}

void drawTriangle(Triangle *t, uint8_t r, uint8_t g, uint8_t b) {
  if (NULL != t) {
    drawLine(&(t->l1), r, g, b);
    drawLine(&(t->l2), r, g, b);    
    drawLine(&(t->l3), r, g, b);    
  }
}

void clear() {
  setAllPixel(0, 0, 0);

  strip.show();
}

/* Effects */

void rotate() {
  for (uint8_t i = 0; i < LENGTH(triangles); i++) {
    drawTriangle(triangles[i], 255, 0, 0);

    strip.show();

    delay(500);

    for (uint8_t j = i; j < LENGTH(triangles); j++) {
      drawTriangle(triangles[j],       0, 0, 0);
      drawTriangle(triangles[j + 1], 255, 0, 0);

      strip.show();

      delay(500);
    }

    drawTriangle(triangles[i], 255, 0, 0);
  }
}

void race() {
  for (uint8_t i = 0; i < LENGTH(lines); i++) {
    for (uint8_t j = lines[i]->start; j < lines[i]->end; j++) {
      setAllPixel(255, 0, 0);
      setPixel(j, 255, 255, 0);
      
      strip.show();

      delay(500);      
    }
  }
}

void hilight() {
  for (uint8_t i = 0; i < LENGTH(high); i++) {
    if (255 == i) {
      strip.show();
      delay(100);  

      setAllPixel(255, 0, 0);
    } else {
      setPixel(high[i], 255, 255, 255);
    }
  }
}

void pulsate() {
  uint8_t treshold = random(255);

  for (uint8_t i = 255; i > treshold; i -= 15) {
    setAllPixel(i, 0, 0);

    strip.show();

    delay(200);
  }

  for (uint8_t i = treshold; i < 255; i += 15) {
    setAllPixel(i, 0, 0);

    strip.show();

    delay(200);
  }
}

/**
 * Set up arduino
 **/
 
void setup() {
  strip.begin();
  strip.show();

#ifdef DEBUG
  Serial.begin(9600);
#endif /* DEBUG */
}

/**
 * Run in loop
 **/

void loop() {
  setAllPixel(255, 0, 0);
  strip.show();

  delay(5000);

  clear();

  switch (random(1, 4)) {
    case 1: rotate();  break;
    case 2: race();    break;    
    case 3: hilight(); break;
    case 4: pulsate(); break;
  }

  delay(1000);
}
