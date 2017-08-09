/**
 * @package witchboard
 *
 * @file Witchboard arduino sketch
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

#define LEDS             44                       ///< Total LEDs
#define CHANNEL_PER_LED  3                        ///< Channel per LED (typically RGB=3)
#define COLORS           (LEDS * CHANNEL_PER_LED) ///< Total colors

/* DMX stuff */
#define STARTADDR        161
#define RX_PIN           3
#define WX_PIN           4

#define CHANNELS         8

/* Presets */
#define RED   255,   0,  0
#define GREEN   0, 255, 255
#define BLUE    0,   0, 255
#define WHITE 255, 255, 255 

/* Witchboard signs */
typedef struct sign_t {
  char               letter;
  short unsigned int lednum;
  short unsigned int ledcount;
} Sign;

Sign signs[] = {
 /* CHR  LED  CNT */
  { 'A', 38,  1 },
  { 'B', 37,  1 },
  { 'C', 36,  1 },
  { 'D', 35,  1 },
  { 'E', 34,  1 },
  { 'F', 33,  1 },
  { 'G', 32,  1 },
  { 'H', 31,  1 },
  { 'I', 30,  1 },
  { 'J', 29,  1 },
  { 'K', 28,  1 },
  { 'L', 27,  1 },
  { 'M', 26,  1 },
  { 'N', 13,  1 },
  { 'O', 14,  1 },
  { 'P', 15,  1 },
  { 'Q', 16,  1 },
  { 'R', 17,  1 },
  { 'S', 18,  1 },
  { 'T', 19,  1 },
  { 'U', 20,  1 },
  { 'V', 21,  1 },
  { 'W', 22,  1 },
  { 'X', 23,  1 },
  { 'Y', 24,  1 },
  { 'Z', 25,  1 },
  { '1', 12,  1 },
  { '2', 11,  1 },
  { '3', 10,  1 },
  { '4',  9,  1 },
  { '5',  8,  1 },
  { '6',  7,  1 },
  { '7',  6,  1 },
  { '8',  5,  1 },
  { '9',  4,  1 },
  { '0',  3,  1 },
  { '!',  0,  3 }, ///< Farewell
  { '/', 39,  2 }, ///< Yes
  { '$', 41,  2 }  ///< No
};

short unsigned int colors[COLORS] = { 0 };

/* Auto mode */
bool auto_mode  = false;
int  auto_delay = 1000;

short unsigned int auto_red   = 255;
short unsigned int auto_green = 255;
short unsigned int auto_blue  = 255;

#define NAUTOMSGS 4

char auto_msgs[NAUTOMSGS][30] = {
  { "HELLO" },
  { "ANYONE OUT THERE" },
  { "HELP ME" },
  { "YOU ARE GOING TO DIE" }
};

DMX_Slave slave(CHANNELS, RX_PIN);

/** updateCols
 *  Write color values to SPI
 **/

void
updateCols()
{
  for(int i = 0; i < COLORS; i++)
    {
      for(SPDR = colors[i]; !(SPSR & _BV(SPIF)); );
    }

  delay(1);
}

 /** setSign
  * Set given sign to given RGB value
  * 
  * @param  sign  A {@link Sign}
  * @param  r     RED value
  * @param  g     GREEN value
  * @param  b     Blue value
  **/

void
setSign(Sign sign,
  short unsigned int r,
  short unsigned int g,
  short unsigned int b)
{
  /* Enable all required LED */
  for(int j = 0; j < sign.ledcount; j++)
    {
      short unsigned int idx = ((sign.lednum + j) * CHANNEL_PER_LED);
  
      colors[idx]     = r;
      colors[idx + 1] = g;
      colors[idx + 2] = b;
    }
}

void
setInRange(short unsigned int a,
  short unsigned int b,
  short unsigned int value)
{
  short unsigned int diff  = (b - a);
  short unsigned int range = (255 / diff);
  short unsigned int idx   = (a + (value / range)) * CHANNEL_PER_LED;

  /* Ignore first range step */
  if(0 < value)
    {
      colors[idx]     = auto_red;
      colors[idx + 1] = auto_green;
      colors[idx + 2] = auto_blue;
    }
}

 /** setLetter
  * Set given letter on board
  * 
  * @param  letter  Letter from auto mode
  * @param  r       RED value
  * @param  g       GREEN value
  * @param  b       Blue value
  **/

void
setLetter(char letter,
  short unsigned int r,
  short unsigned int g,
  short unsigned int b)
{  
  memset(colors, 0, sizeof(colors)); ///< Reset colors

  /* Exclude blanks here */
  if(' ' != letter) 
    {
      for(int i = 0; i < sizeof(signs) / sizeof(signs[0]); i++)
        {
          Sign sign = signs[i];
                  
          if(letter == sign.letter)
            {
              setSign(sign, r, g, b);

              break;
            }
        }
    }

  updateCols();
}

/** onFrameReceived
 * Handle frame receival
 *  
 * @param  channels  Number of set channels
 **/

void
onFrameReceived(short unsigned int channels)
{
  /* Set mode */
  int value = slave.getChannelValue(1);

  auto_mode = (127 < value);

  if(!auto_mode) memset(colors, 0, sizeof(colors)); ///< Reset colors

  /* Set speed */
  value = slave.getChannelValue(2);

  auto_delay = 1020 - (4 * value); ///< Invert
  
  /* Set colors */
  auto_red   = slave.getChannelValue(3);
  auto_green = slave.getChannelValue(4);
  auto_blue  = slave.getChannelValue(5);

  /* Set A-M */
  value = slave.getChannelValue(6);

  setInRange(26, 38, value);

  /* Set N-Z */
  value = slave.getChannelValue(7);

  setInRange(13, 25, value);

  /* Set 1-9-0 */
  value = slave.getChannelValue(8);

  setInRange(3, 12, value);

  if(!auto_mode) updateCols();
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
  slave.enable();  
  slave.setStartAddress(STARTADDR);
  slave.onReceiveComplete(onFrameReceived);
}

/** loop
 * Run in loop
 **/

void
loop()
{
  /* Run auto_mode when enabled */
  if(auto_mode)
    { 
      for(int i = 0; i < NAUTOMSGS; i++)
        {
          for(int a = 0; a < strlen(auto_msgs[i]); a++)
            {
              if(auto_mode)
                {
                  setLetter(auto_msgs[i][a], auto_red,
                    auto_green, auto_blue);
                }
              
              delay(auto_delay);
            }

          setLetter('!', auto_red, auto_green, auto_blue);

          delay(auto_delay);
        }
    }
}
