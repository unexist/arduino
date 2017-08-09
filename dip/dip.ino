/**
 * @package dip
 *
 * @file Dipswitch arduino sketch
 * @copyright (c) 2016 Christoph Kappel <unexist@subforge.org>
 * @version $Id$
 *
 * This program can be distributed under the terms of the GNU GPL.
 * See the file COPYING.
 **/

#define arySize(ary) (sizeof(ary) / sizeof(ary[0]))

/* Digital inputs */
char pins[] = { 7, 6, 5, 4, 3, 2 }; ///< Reverse order
 
void
setup()
{
  /* Set up serial */
  Serial.begin(9600);
  Serial.println("Serial port open");
  
  /* Set up input pins */
  for(int i = 0; i < arySize(pins); i++)
    {
      pinMode(pins[i], INPUT_PULLUP);
    }
}

void
loop()
{
  int addr = 0;
  
  for(int i = 0; i < arySize(pins); i++)
    {
      addr = (addr << 1) | !digitalRead(pins[i]);
    }

  Serial.println(addr);
}
