/**
 * @package vucactus
 *
 * @file Vucactus arduino sketch
 * @copyright (c) 2016 Christoph Kappel <unexist@subforge.org>
 * @version $Id$
 *
 * This program can be distributed under the terms of the GNU GPL.
 * See the file COPYING.
 **/

#include <SPI.h>

/* MSGEQ7 stuff */
#define ANALOG_PIN A0
#define STROBE_PIN 2
#define RESET_PIN 3

int values[7];

/* LED stuff */
#define LED_DDR   DDRB
#define LED_PORT  PORTB
#define LED_PIN   (1 << PORTB5)

#define LEDS      20
#define COLORS    (LEDS * 3)

char colors[COLORS] = { 0 };

/* Debug */
//#define DEBUG 1

/* Cactus levels */
typedef struct level_t {
  short unsigned int treshhold;
  short unsigned int r;
  short unsigned int g;
  short unsigned int b;
  short int rows[4];
} Level;

Level levels[] {
  /* Treshhold   R    G    B     #1  #2  #3  #4 */
  {     155,      0, 255,   0, { 19,  0, -1, -1 }}, ///< Green
  {     300,      0, 255,   0, { 18,  1,  2, -1 }},
  {     450,      0, 255,   0, { 17, -1, -1, -1 }},
  {     600,      0, 255,   0, { 15, 16,  6,  5 }},
  {     750,      0, 255,   0, { 14, 12,  4, -1 }},
  
  {     900,    255, 255,   0, { 13,  7,  3, -1 }}, ///< Yellow
  {     950,    255, 255,   0, { 11,  8, -1, -1 }},
  
  {    1000,    255,   0,   0, { 10,  9, -1, -1 }}  ///< Red
};

/**
 * Transfer data via SPI and wait for finish
 * 
 * @param[in]  data  Data to write
 **/

void
transfer(volatile char data)
{
  /* Copy data */
  SPDR = data;

  /* Poll bit and wait for end of the transmission */
  while(!(SPSR & (1 << SPIF)));
}

/**
 *  Write color values to SPI
 **/

void
updateCols()
{
  for(short unsigned int i = 0; i < COLORS; i++)
    {
      transfer(colors[i]);
    }
}

/**
 * Clear columns
 */

void
clearCols()
{
  for(short unsigned int i = 0; i < COLORS; i++)
    {
      colors[i] = 0;
    }  
}

/**
 * Set color of led
 * 
 * @param[in]  led  Number of LED
 * @param[in]  r    Value of red
 * @param[in]  g    Value of green
 * @param[in]  b    Value of blue
 **/

void
setColor(short unsigned int led,
  short unsigned int r,
  short unsigned int g,
  short unsigned int b)
{
  int pos = (led * 3);
  
  colors[pos]     = r;
  colors[pos + 1] = g;
  colors[pos + 2] = b; 
}

/**
 * Sets given vu level
 * 
 * @param[in]  level   Level number
 * @param[in]  value   Value for level
 * @param[in]  reset   Whether to reset level when treshhold doesn't match
 **/

void
setLevel(short unsigned int level,
  short unsigned int value,
  boolean reset)
{
  Level *l = &levels[level];
  
  for(int i = 0; i < 4; i++) {
    if(-1 != l->rows[i]) {
      if(value >= l->treshhold) {
        setColor(l->rows[i], l->r, l->g, l->b); 
      } else if (reset) {
        setColor(l->rows[i], 0, 0, 0);       
      }
    }
  }
}

/**
 * Set up arduino
 **/
 
void 
setup()
{
  /* Init LED and SPI */
  LED_DDR  |=  LED_PIN; ///< Enable output for LED
  LED_PORT &= ~LED_PIN; ///< LED off

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV16); ///< 1 MHz max, else flicker

  updateCols();  

#ifdef DEBUG
  Serial.begin(9600);
#endif /* DEBUG */

  /* Pin setup: MSGEQ7 */
  pinMode(ANALOG_PIN, INPUT);
  pinMode(STROBE_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);

  analogReference(DEFAULT);

  /* Init MSGEQ7 */
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STROBE_PIN, HIGH);
}

/**
 * Run in loop
 **/

void
loop()
{
  int i;
  
  /* Reset MSGEQ7 */
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(RESET_PIN, LOW);

  for(i = 0; i < 7; i++)
    {
      digitalWrite(STROBE_PIN, LOW);
      delayMicroseconds(30); ///< Allow the output to settle
      
      values[i] = analogRead(ANALOG_PIN);

#ifdef DEBUG
      Serial.print(" ");
      Serial.print(values[i]);
#endif /* DEBUG */

      digitalWrite(STROBE_PIN, HIGH); ///< Clock multiplexer
   }

#ifdef SERIAL   
 Serial.println();
#endif /* SERIAL */

 clearCols();

 /* Set leds */
 for(i = 0; i < 7; i++)
   {     
     setLevel(7, values[i], false);
     setLevel(6, values[i], false);
     setLevel(5, values[i], false);
     setLevel(4, values[i], false);
     setLevel(3, values[i], false);
     setLevel(2, values[i], false);
     setLevel(1, values[i], false);
     setLevel(0, values[i], false);
   }

 updateCols();
}
