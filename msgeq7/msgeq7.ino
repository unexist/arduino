/**
   @package msgeq7

   @file MSGEQ7 arduino sketch
   @copyright (c) 2018-present Christoph Kappel <christoph@unexist.dev>
   @version $Id$

   This program can be distributed under the terms of the GNU GPLv3.
   See the file COPYING.
 **/

#define ANALOG_PIN A0
#define STROBE_PIN 2
#define RESET_PIN 3

int values[7];
int leds[7] = { 5, 6, 7, 8, 9, 10, 11 };
int i;

void setup()
{
  Serial.begin(9600);

  /* Pin setup: MSGEQ7 */
  pinMode(ANALOG_PIN, INPUT);
  pinMode(STROBE_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);

  /* Pin setup: LEDs */
  for(i = 0; i < 7; i++)
    pinMode(leds[i], OUTPUT);

  analogReference(DEFAULT);

  /* Init MSGEQ7 */
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STROBE_PIN, HIGH);
}

void loop()
{
  /* Reset MSGEQ7 */
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(RESET_PIN, LOW);

  for(i = 0; i < 7; i++)
    {
      digitalWrite(STROBE_PIN, LOW);
      delayMicroseconds(30); ///< Allow the output to settle
      
      values[i] = analogRead(ANALOG_PIN);

      Serial.print(" ");
      Serial.print(values[i]);

      digitalWrite(STROBE_PIN, HIGH); ///< Clock multiplexer
   }
   
 Serial.println();

 /* Set leds */
 for(i = 0; i < 7; i++)
   {
     if(values[i] > 500)
       {
          //digitalWrite(leds[0], HIGH);
       }
     else digitalWrite(leds[i], LOW);
   }
}
