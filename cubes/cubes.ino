/**
 * @package cubes
 *
 * @file Cubes arduino sketch
 * @copyright (c) 2016 Christoph Kappel <unexist@subforge.org>
 * @version $Id$
 *
 * This program can be distributed under the terms of the GNU GPL.
 * See the file COPYING.
 **/

#include <SPI.h>
#include <Conceptinetics.h>

/* LED stuff */
#define LED_DDR          DDRB
#define LED_PORT         PORTB
#define LED_PIN          (1 << PORTB5)

#define GROUPS           6                                          ///< Total LED groups
#define LED_PER_GROUP    6                                          ///< LEDs per group
#define CHANNEL_PER_LED  3                                          ///< Channel per LED (typically RGB=3)
#define CONTROL_CHAN     4                                          ///< Number of special channel
#define CHANNELS         (GROUPS * CHANNEL_PER_LED + CONTROL_CHAN)  ///< Total channel for DMX
#define COLORS           (GROUPS * CHANNEL_PER_LED * LED_PER_GROUP) ///< Total colors

char colors[COLORS] = { 0 };

/* DMX stuff */
#define STARTADDR  289
#define RX_PIN     3
#define WX_PIN     4

DMX_Slave slave(CHANNELS, RX_PIN);

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

/** setUnlessZero
 * Set value when not zero
 * @param  color  Color var
 * @param  value  New value
 **/

void
setUnlessZero(char *color, 
  char value)
{
  if(0 != value) *color = value;
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

/** onFrameReceived
 *  Handle frame receival
 *  @param  channels  Channels that was set
 **/

void
onFrameReceived(short unsigned int channel)
{
  /* 1) Set colors for color groups */
  for(char led = 0, pos = 0, chan = (CONTROL_CHAN + 1); led < GROUPS; led++) ///< Channel count starts at 1!
    {     
      for(char posoff = 0, chanoff = 0; posoff < (LED_PER_GROUP * CHANNEL_PER_LED); posoff++)
        {
          colors[pos + posoff] = slave.getChannelValue(chan + chanoff++);

          if(CHANNEL_PER_LED == chanoff) chanoff = 0;
        }

      chan += CHANNEL_PER_LED;
      pos  += (LED_PER_GROUP * CHANNEL_PER_LED);
    }

  /* 2) Control: Set color for all LEDs */
  for(char pos = 0; pos < COLORS; pos += CHANNEL_PER_LED)
    {
      setUnlessZero(&colors[pos],     slave.getChannelValue(2));
      setUnlessZero(&colors[pos + 1], slave.getChannelValue(3));
      setUnlessZero(&colors[pos + 2], slave.getChannelValue(4));
    }

  /* 3) Control: Special mode */
  char  mode   = slave.getChannelValue(1);
  short cols[] = { 0, 0, 255, 0, 0, 255, 0 };
  char  sel    = 0;

  for(char gid = 0; 10 <= mode && gid < GROUPS; gid++)
    {
      char mod = (gid % CHANNEL_PER_LED);

      /* Pick selector function */
      if(10 <= mode && 20 > mode)
        {
          sel = 4 - mod; ///< G-B-R
        }
      else if(20 <= mode && 30 > mode)
        {
          sel = 3 - mod; ///< B-R-G
        }
      else if(30 <= mode && 40 > mode)
        {
          sel = 2 - mod; ///< R-G-B
        }
      else if(40 <= mode && 50 > mode)
        { 
          sel = mod; ///< B-G-R         
        }
      else if(50 <= mode && 60 > mode)
        {          
          sel = 1 + mod; ///< G-R-B
        }
      else if(60 <= mode && 70 > mode)
        {          
          sel = 2 + mod; ///< R-B-G
        }
      /* Solid colors */
      else if(210 <= mode && 220 > mode)
        {
          sel = 2; ///< R
        }
      else if(220 <= mode && 230 > mode)
        {
          sel = 1; ///< G
        }
      else if(230 <= mode && 255 > mode)
        {
          sel = 0; ///< B
        }

      setForGroup(gid, cols[sel], cols[sel + 1], cols[sel + 2]);
    }

  updateCols(); 
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

  /* Init and start slave */
  slave.enable ();  
  slave.setStartAddress(STARTADDR);
  slave.onReceiveComplete(onFrameReceived);
}

/** loop
 * Run in loop
 **/

void
loop()
{
  /* We do nothing here */
}
