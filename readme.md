# VX8GPS-nano33
Code and instructions for building a DIY GPS module for the VX-8 handheld transceiver using an Arduino Nano 33.

## Why does this project exist? Isn't there code out there already that does this?

The short answer: Yes, there is arduino code that does this already, but it doesn't work on any variant of the Arduino Nano 33. 

The long answer: 

This project originally started with me attempting to complete the build outlined [on this github page by dfannin.](https://github.com/dfannin/arduino-vx8r-gps) I tried to build it with one modification: To use a 3.3V arduino so that I don't need an external battery pack for it. (The VX-8 normally provides 3.3V to the OEM GPS module, something that this build takes advantage of.) The only problem was that the Nano 33 hardware is very different from the Nano v3 used in the original build. It turns out that there's no version of the software serial port library that it uses for the processor architecture of the Nano 33. I then found [this code](https://github.com/WaterstarRunner/VX8GPS-Xiao) for the Seeeduino Xiao that's compatible with SAMD processors, and modified the code from there to add a software serial port, because the Nano 33 only has one hardware serial port. In addition to all that, powering the Nano 33 directly with 3.3V requires the power to be connected directly to the 3.3V pin rather than the VIN pin. This bypasses the voltage regulator, because otherwise the voltage drop from the regulator prevents the nano from turning on. 

## Sources

- [The original project that I started with](https://github.com/dfannin/arduino-vx8r-gps)
- [The code that I modified for this project](https://github.com/WaterstarRunner/VX8GPS-Xiao)
- [The stackoverflow thread I referenced when adding the code for a software serial port](https://stackoverflow.com/questions/57175348/softwareserial-for-arduino-nano-33-iot)
- 
## Parts List

  - VX-8 handheld transceiver (duh) 
  - CT-M11 microphone cable
  - Arduino Nano 33 (the Nano 33 IoT and the Nano 33 Bluetooth should both work. The IoT and BT features are not used in this build.
  - NEO-6M GPS module (Note that you want the module with the chip and other electronics on a PCB as well as an external antenna, not just the NEO-6M chip.)
  - Soldering equipment and wire
  - Micro-USB cable for the arduino

## Assembly

