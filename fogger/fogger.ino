/**
   @package fogger

   @file Fogger arduino sketch
   @copyright (c) 2017-present Christoph Kappel <christoph@unexist.dev>
   @version $Id$

   This program can be distributed under the terms of the GNU GPLv3.
   See the file COPYING.
 **/

#define PIN_RELAIS1    7
#define PIN_SONIC_ECHO 4
#define PIN_SONIC_TRIG 3
#define PIN_ARDUINO    13

/* Debug */
#define DEBUG 1

void setup() {
  /* Set up pins */
  pinMode(PIN_RELAIS1, OUTPUT);
  pinMode(PIN_SONIC_ECHO, INPUT);   
  pinMode(PIN_SONIC_TRIG, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_ARDUINO, OUTPUT);

  /* Set Relais to high */
  digitalWrite(PIN_RELAIS1, HIGH);

  /* Set internal led to high */
  digitalWrite(LED_BUILTIN, HIGH);

  /* Set Arduino to low */
  digitalWrite(PIN_ARDUINO, LOW);

#ifdef DEBUG
  Serial.begin(9600);
#endif /* DEBUG */
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

#ifdef DEBUG
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
#endif /* DEBUG */

  if (110 >= distance && 50 <= distance) {
    /* Notify other Arduino */
    digitalWrite(PIN_ARDUINO, HIGH);
      
    /* Enable fogger */
    digitalWrite(PIN_RELAIS1, LOW);
    delay(5000);
    digitalWrite(PIN_RELAIS1, HIGH);

#ifdef DEBUG
    Serial.println("Fogger sleep 15min");
 #endif /* DEBUG */

    /* Set internal LED, notify other Arduino and sleep 15mins */
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(PIN_ARDUINO, LOW);
    delay(900000);
    digitalWrite(LED_BUILTIN, LOW);
  }
 
  delay(100);
}
