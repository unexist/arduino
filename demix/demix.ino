/**
 * @package demix
 *
 * @file Demix arduino sketch
 * @copyright (c) 2015-present Christoph Kappel <christoph@unexist.dev>
 * @version $Id$
 *
 * This program can be distributed under the terms of the GNU GPLv3.
 * See the file COPYING.
 **/

#include <DmxSimple.h>

int value   = 0;
int channel = 0;

/** setup
 * Set up arduino
 **/
  
void
setup()
{
  /* Set up lib */
  DmxSimple.usePin(4);
  DmxSimple.maxChannel(128);
  
  /* Set shield to output mode */
  pinMode (2, OUTPUT);
  digitalWrite (2, HIGH);

  /* Set up serial */
  Serial.begin(9600);
}

/** loop
 * Run in loop
 **/

void
loop()
{
  int c;

  while(!Serial.available());

  c = Serial.read();

  /* Convert char to int */
  if(('0' <= c) && ('9' >= c))
    {
      value = 10 * value + c - '0';
    }
  else ///< Handle delimiter
    {
      if('(' == c) channel = value;
      else if (')' == c)
        {
          DmxSimple.write(channel, value);
        }
        
      value = 0;
    }
}
