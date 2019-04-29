// driver for the GSL1680 touch panel
// Information gleaned from https://github.com/rastersoft/gsl1680.git and various other sources
// firmware for the specific panel was found here:- http://www.buydisplay.com/default/5-inch-tft-lcd-module-800x480-display-w-controller-i2c-serial-spi
// As was some test code.
// This is for that 800X480 display and the 480x272 from buydisplay.com
// Thanks to wolfmanjm for the source code this lib is bassed on : https://github.com/wolfmanjm/GSL1680

/*
Pin outs
the FPC on the touch panel is six pins, pin 1 is to the left pin 6 to the right with the display facing up

pin | function  | Arduino Uno
-----------------------------
1   | SCL       | A5
2   | SDA       | A4
3   | VDD (3v3) | 3v3
4   | Wake      | 4
5   | Int       | 2
6   | Gnd       | gnd
*/

#include "GSL1680.h"

// Pins
#define WAKE 4
#define INTRPT 2

GSL1680 TS = GSL1680();
// An other constructor is available to enable some debug on serial : GSL1680(bool error, bool info);

void setup () {
  Serial.begin(9600);
  delay(1000);
  TS.begin(WAKE, INTRPT);                 // Startup sequence CONTROLER part
}

void loop () {
  if(digitalRead(INTRPT) == HIGH) {
    Serial.println("Touch");
    int NBFinger = TS.dataread();
    for(int i=0; i<NBFinger; i++){
      Serial.print(TS.readFingerID(i));
      Serial.print(" ");
      Serial.print(TS.readFingerX(i));
      Serial.print(" ");
      Serial.print(TS.readFingerY(i));
      Serial.println("");
    }
    Serial.println("---");
  }
}
