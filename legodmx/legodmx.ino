/**
 * @package legodmx
 *
 * @file Legodmx arduino sketch
 * @copyright (c) 2025-present Christoph Kappel <christoph@unexist.dev>
 * @version $Id$
 *
 * This program can be distributed under the terms of the GNU GPLv3.
 * See the file LICENSE.
 **/

#include <Conceptinetics.h>

//#define DEBUG 1

/* Mosfet stuff */
#define MOS_PIN       6

/* DMX stuff */
#define DMX_ADDRESS   1
#define DMX_CHANNELS  1
#define DMX_RX_PIN    3
#define DMX_WX_PIN    4

DMX_Slave slave(DMX_CHANNELS, DMX_RX_PIN);

#ifdef DEBUG
char buf[255] = { 0 };
#endif /* DEBUG */

/**
 *  Handle frame receival
 *  @param  channels  Channels that was set
 **/

void onReceiveComplete(short unsigned int channel) {
    int dmxval = slave.getChannelValue(1);
  
    analogWrite(MOS_PIN, dmxval);

#ifdef DEBUG
    snprintf(buf, sizeof(buf), "chan=%d, val=%d\n", channel, dmxval);

    Serial.print(buf);
#endif /* DEBUG */
}

/**
 * Set up arduino
 **/
 
void setup() {
    /* Init pins */
    pinMode(MOS_PIN, OUTPUT);

#ifdef DEBUG
    /* Init and start DMX slave */
    slave.enable();  
    slave.setStartAddress(DMX_ADDRESS);
    slave.onReceiveComplete(onReceiveComplete);
#else DEBUG /* DEBUG */
    Serial.begin(9600);
#endif /* DEBUG */
}

/**
 * Run in loop
 **/

void loop() {
    /* We do nothing here */
}
