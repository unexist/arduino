/**
   @package fogger

   @file Fogger arduino sketch
   @copyright (c) 2017 Christoph Kappel <unexist@subforge.org>
   @version $Id$

   This program can be distributed under the terms of the GNU GPL.
   See the file COPYING.
 **/

#define PIN_RELAIS1    7
#define PIN_SONIC_ECHO 4
#define PIN_SONIC_TRIG 3

void setup() {
  /* Set up  pins */
  pinMode(PIN_RELAIS1, OUTPUT);
  pinMode(PIN_SONIC_ECHO, INPUT);   
  pinMode(PIN_SONIC_TRIG, OUTPUT);

  /* Set Relais to high */
  digitalWrite(PIN_RELAIS1, HIGH);

  Serial.begin(9600);
}

void loop() {
  /* Reset pins of sonic detector and trigger check */
  digitalWrite(PIN_SONIC_TRIG, LOW);
  delay(5);
  digitalWrite(PIN_SONIC_TRIG, HIGH);
  delay(10);
  digitalWrite(PIN_SONIC_TRIG, LOW);

  /* Detection */
  long duration = pulseIn(PIN_SONIC_ECHO, HIGH);
  long distance = (duration / 2) * 0.03432;

  if (200 >= distance && 0 != distance)
    {
      digitalWrite(PIN_RELAIS1, LOW);
      delay(4000);
      digitalWrite(PIN_RELAIS1, HIGH);

      delay(600000);
    }

  Serial.print(distance);
  Serial.println(" cm");

  delay(100);
}
