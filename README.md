# arduino-eprom-programmer
GNU-Linux/Windows C++ Qt 5 Application for Arduino Eprom Programmer 

Tool for programming SNES and ZX Spectrum roms using an Arduino Mega with a custom hardware add-on, for retrocomputing or whatever. It is easily modifiable to include more types of EPROMs.

## Usage:
The program allows reading/writing from/to the EPROM. Originally designed by Robson Couto, I modified his Arduino sketch and his hardware design to allow 27256K ZXSpectrum EPROMs to be read/written (few cables and some jumpers). 

I developed my own Qt application to comunicate with the device and give the project a fancy look.

## Compilation notes:
- The program was developed using Qt 5.10 and tested in Windows 7 using MinGW 32bits compiler. 

- Once compiled, Qt will generate a release/debug folder for the binaries,  "graphics" folder it is needed to be copied into it manually.

## Project
Original project by Robson Couto, all credit, thanks (and complains) to him:

https://github.com/robsoncouto/eprom

http://dragaosemchama.com/2016/10/arduino-mega-eprom-programmer/

Probando
