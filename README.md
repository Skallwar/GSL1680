# GSL1680
Arduino driver for GSL1680

Information gleaned from https://github.com/rastersoft/gsl1680.git and various other sources
Firmware for the specific panel was found here:- http://www.buydisplay.com/default/5-inch-tft-lcd-module-800x480-display-w-controller-i2c-serial-spi
Use source code from wolfmanjm : https://github.com/wolfmanjm/GSL1680

pin | function  | Arduino Uno
----|-----------|------------
1   | SCL       | A5
2   | SDA       | A4
3   | VDD (3v3) | 3v3
4   | Wake      | 4
5   | Int       | 2
6   | Gnd       | gnd

NOTE : Firmware is really heavy so this library cannot be used in some Arduino boards
