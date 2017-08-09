/**
 * @package stairway
 *
 * @file Cubes arduino sketch
 * @copyright (c) 2017 Christoph Kappel <unexist@subforge.org>
 * @version $Id$
 *
 * This program can be distributed under the terms of the GNU GPL.
 * See the file COPYING.
 **/

#include <SPI.h>

/* LED stuff */
#define LED_DDR          DDRB
#define LED_PORT         PORTB
#define LED_PIN          (1 << PORTB5)

#define GROUPS           5                                         ///< Total LED groups
#define LED_PER_GROUP    2                                          ///< LEDs per group
#define CHANNEL_PER_LED  3                                          ///< Channel per LED (typically RGB=3)
#define COLORS           (GROUPS * CHANNEL_PER_LED * LED_PER_GROUP) ///< Total colors

char colors[COLORS] = { 0 };

/* Presets */
#define RED   255,   0,  0
#define GREEN   0, 255, 255
#define BLUE    0,   0, 255

/* transfer
 * Transfer data via SPI and wait for finish
 * @param  data  Data to write
 **/

void
transfer(volatile char data)
{
  /* Copy data */
  SPDR = data;

  /* Poll bit and wait for end of the transmission */
  while(!(SPSR & (1 << SPIF)));
}

/** updateCols
 *  Write color values to SPI
 **/

void
updateCols()
{
  for(char i = 0; i < COLORS; i++)
    {
      transfer(colors[i]);
    }
}

/** setForGroups
 * Set RGB color for given group
 * @param  gid  ID of group to set colors
 * @param  r    Red value
 * @param  g    Green value
 * @param  b    Blue value
 **/

void
setForGroup(int gid,
  char r,
  char g,
  char b)
{
  for(char curgid = 0, pos = (gid * LED_PER_GROUP * CHANNEL_PER_LED);
      curgid < LED_PER_GROUP; curgid++, pos += CHANNEL_PER_LED)
    {
      colors[pos]     = r;
      colors[pos + 1] = g;
      colors[pos + 2] = b;
    }
}

/** setup
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
}

/** loop
 * Run in loop
 **/

void
loop()
{
  setForGroup(0, RED);  
  setForGroup(1, GREEN);
  setForGroup(2, BLUE);

  updateCols();
  /* We do nothing here */
}
