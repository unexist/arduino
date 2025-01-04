/**
 * @package skull
 *
 * @file Skull arduino sketch
 * @copyright (c) 2018-present Christoph Kappel <christoph@unexist.dev>
 * @version $Id$
 *
 * This program can be distributed under the terms of the GNU GPLv3.
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

#define LEDS      5
#define COLORS    (LEDS * 3)

char colors[COLORS] = { 0 };

/* Presets */
#define RED   255,   0,  0
#define GREEN   0, 255, 255
#define BLUE    0,   0, 255

/* Debug */
#define DEBUG 1

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
 * Write color values to SPI
 **/

void
updateCols()
{
  for(char i = 0; i < COLORS; i++)
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

#define LEFT 0
#define RIGHT 1

void
setEye(char side,
  char r,
  char g,
  char b)
{
  char idx = (LEFT == side ? 9 : 12);

  colors[idx]     = r; 
  colors[idx + 1] = g;  
  colors[idx + 2] = b;  
}

void
setBase(char pos,
  char r,
  char g,
  char b)
{
  int idx = pos * 3;
  
  colors[idx]     = r; 
  colors[idx + 1] = g;  
  colors[idx + 2] = b;
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

  setEye(LEFT, RED);
  setEye(RIGHT, RED);

  setBase(0, RED);
  setBase(1, GREEN);
  setBase(2, BLUE);

  updateCols();
}

/** loop
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
}
