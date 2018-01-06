#include "fix_fft.h"

#define PIN A0
#define NSAMPLES 128

char im[NSAMPLES];
char data[NSAMPLES];

void setup() {
  Serial.begin(9600);
}

void loop() {
  int static i = 0;
  static long tt;
  int val;

  if (millis() > tt) {
    if (i < NSAMPLES) {
      val = analogRead(PIN);
      data[i] = val / 4 - NSAMPLES;
      im[i] = 0;
      i++;
    } else {
      fix_fft(data, im, 7, 0);

      // I am only interessted in the absolute value of the transformation
      for (i = 0; i < 64; i++) {
        data[i] = sqrt(data[i] * data[i] + im[i] * im[i]);
      }

      //do something with the data values 1..64 and ignore im
      //show_big_bars(data,0);
      Serial.println(data[0]);
    }

    tt = millis();
  }
}
