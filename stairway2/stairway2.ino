
/**
 * @package stairway
 *
 * @file Stairway arduino sketch
 * @copyright (c) 2017 Christoph Kappel <unexist@subforge.org>
 * @version $Id$
 *
 * This program can be distributed under the terms of the GNU GPL.
 * See the file COPYING.
 **/
 
#include <Adafruit_NeoPixel.h>
#include <Conceptinetics.h>

/* LED stuff */
#define PIN 6
#define NROWS          18
#define PIXEL_PER_ROW  23
#define PIXELS         (NROWS * PIXEL_PER_ROW)
#define FADERATE       0.95
#define DELAY          30

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);

/* DMX stuff */
#define STARTADDR  217
#define RX_PIN     3
#define WX_PIN     4
#define CHANNELS   6

#define CHANNEL_RED    1
#define CHANNEL_GREEN  2
#define CHANNEL_BLUE   3
#define CHANNEL_SPEED  4
#define CHANNEL_DELAY  5
#define CHANNEL_BRIGHT 6

DMX_Slave slave(CHANNELS, RX_PIN);

byte dmx_red    = 0;
byte dmx_green  = 0; 
byte dmx_blue   = 0; 
byte dmx_speed  = 0; 
byte dmx_delay  = 0;
byte dmx_bright = 0;

/** onFrameReceived
 *  Handle frame receival
 *  @param  channels  Channels that was set
 **/

void
onFrameReceived(short unsigned int channel)
{
  dmx_red    = slave.getChannelValue(CHANNEL_RED);
  dmx_green  = slave.getChannelValue(CHANNEL_GREEN);
  dmx_blue   = slave.getChannelValue(CHANNEL_BLUE);
  dmx_speed  = slave.getChannelValue(CHANNEL_SPEED);
  dmx_delay  = slave.getChannelValue(CHANNEL_DELAY);
  dmx_bright = slave.getChannelValue(CHANNEL_BRIGHT);

  if(0 != dmx_bright) strip.setBrightness(dmx_bright);
}

/** setup
 * Set up arduino
 **/
 
void 
setup()
{
  /* Init strip */
  strip.begin();
  strip.show();

  /* Init and start slave */
  slave.enable ();  
  slave.setStartAddress(STARTADDR);
  slave.onReceiveComplete(onFrameReceived);
}

byte
limitVal(byte val,
  bool userset)
{
  return (userset ? val : random(256));
}

byte
dimVal(byte val)
{
  if(1 < val)
    {
      return val * FADERATE;
    }
  else return 0;
}

/** loop
 * Run in loop
 **/

void
loop ()
{
  /* Set dimmer of random pixel dependend on speed value */  
  bool userset = (0 != dmx_red || 0 != dmx_green || 0 != dmx_blue);

  for(uint16_t i = 0; i < dmx_speed + 1; i++)
    {    
      strip.setPixelColor(random(PIXELS),
        limitVal(dmx_red,   userset), 
        limitVal(dmx_green, userset), 
        limitVal(dmx_blue,  userset));
    }

    /* Update all pixels */
    for(uint16_t i = 0; i < PIXELS; i++)
      {
        /* Fetch color and unpack */
        uint32_t c = strip.getPixelColor(i);

        byte r = ((c >> 16) & 0xFF); 
        byte g = ((c >> 8) & 0xFF); 
        byte b = (c & 0xFF);

        /* Dim colors */
        strip.setPixelColor(i, dimVal(r), dimVal(g), dimVal(b));
    }
    
    strip.show();
    delay(0 == dmx_delay ? DELAY : dmx_delay);
}
