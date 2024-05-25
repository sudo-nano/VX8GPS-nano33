# VX8GPS-nano33
Code and instructions for building a DIY GPS module for the VX-8 handheld transceiver using an Arduino Nano 33.

## Why does this project exist? Isn't there code out there already that does this?

The short answer: Yes, there is arduino code that does this already, but it doesn't work on any variant of the Arduino Nano 33. 

The long answer: 

This project originally started with me attempting to complete the build outlined [on this github page by dfannin.](https://github.com/dfannin/arduino-vx8r-gps) I tried to build it with one modification: To use a 3.3V arduino so that I don't need an external battery pack for it. (The VX-8 normally provides 3.3V to the OEM GPS module, something that this build takes advantage of.) The only problem was that the Nano 33 hardware is very different from the Nano v3 used in the original build. It turns out that there's no version of the software serial port library that it uses for the processor architecture of the Nano 33. I then found [this code](https://github.com/WaterstarRunner/VX8GPS-Xiao) for the Seeeduino Xiao that's compatible with SAMD processors, and modified the code from there to add a software serial port, because the Nano 33 only has one hardware serial port. In addition to all that, powering the Nano 33 directly with 3.3V requires the power to be connected directly to the 3.3V pin rather than the VIN pin. This bypasses the voltage regulator, because otherwise the voltage drop from the regulator prevents the nano from turning on. 

## Sources

- [The original project that I started with](https://github.com/dfannin/arduino-vx8r-gps)
- [The code that I modified for this project](https://github.com/WaterstarRunner/VX8GPS-Xiao)
- [The blog post associated with the above code](https://blog.waterstar.run/2021/03/a-simplified-vx-8dr-gps-design.html) (I didn't actually find this blog post until after the project was completed)
- [The stackoverflow thread I referenced when adding the code for a software serial port](https://stackoverflow.com/questions/57175348/softwareserial-for-arduino-nano-33-iot)

## Parts List

  - VX-8 handheld transceiver
  - CT-M11 microphone cable
  - Arduino Nano 33 (the [Nano 33 IoT](https://store.arduino.cc/products/arduino-nano-33-iot) is what I used for this build. I don't know whether the Nano 33 BT works for this, so I recommend using the Nano IoT. The IoT and BT features are not used here.)
  - NEO-6M GPS module (Note that you want the module with the chip and other electronics on a PCB as well as an external antenna, not just the NEO-6M chip.)
  - Soldering equipment and wire
  - Micro-USB cable for the arduino

## Assembly
I know the Arduino Nano 33 has holes instead of pins unless you solder the headers on, but I'll be calling them "pins" even though I mean the solder pads that can optionally have pins. When top and bottom of the arduino are referenced, the orientation of the arduino is with the USB port at the top and oriented towards you, rather than having the other side of the PCB oriented towards you.
### Wiring the CT-M11 cable to the arduino
The labels in parentheses are how the wires in the CT-M11 cable are identified on the radio side of the cable.
- Solder the red wire from the cable (ground) to one of the ground pins on the arduino. This is the ground wire. 
- Solder the blue wire from the cable (GPS RX) to the pin labeled "TX", which is in the bottom right. The arduino will transmit over this pin to relay coordinates to the radio. 
- Solder the green wire from the cable (GPS TX) to the pin labeled "RX", which is right above the "TX" pin. 
- Solder the gray wire from the cable (3.3V) to the 3.3V pin on the arduino, which is towards the top left. 

Note: The TX and RX pins on the arduino are the dedicated hardware serial port, referenced as Serial1 in the code. 

### Wiring the NEO-6M GPS Module
![image](https://user-images.githubusercontent.com/63484662/132972902-0a9080c7-313a-459b-a84a-74c99b84dade.png)

The GPS module also has holes rather than pins unless you add headers, but you know what I mean.
- Wire the ground pin of the GPS module to the same ground pin on the arduino that you connected to the CT-M11 cable's ground. 
- Wire the VCC pin of the GPS module to the 3.3V pin on the arduino where you connected the 3.3V from the radio. 
- Wire the TXD pin on the GPS module to the pin labeled "D5" on the arduino.
- Wire the RXD pin on the GPS module to the pin labeled "D6" on the arduino.
- Leave the PPS pin unconnected. Some modules don't have this, which is ok. 
You have now completed all the wiring. 

### Programming the arduino
Plug the arduino nano 33 into your computer and open the vx8-gps-nano33.ino file in the arduino IDE. If you haven't already, go to Tools > Board > Board Manager (At the top of the window for Windows, top of the screen for OSX) and install the "Arduino SAMD Boards" package via the board manager. After that, go to Tools > Board and select the board you're using. It should be the Nano 33 IoT. If you are using the Nano 33 BT, you may have to install the Arduino Mbed OS boards package. 

Now select the correct USB port by going to Tools > Port and selecting the port showing your arduino. What this looks like varies by OS. After doing this, click the "Upload" button in your Arduino IDE, which should be a right facing arrow that's in the top left of the window. Once it says that it's finished, you should be good to go. 

### Optional: Configuring the GPS module
You may need to reconfigure your GPS module to make it produce the desired NMEA formatted GPS strings. I didn't have to do this with mine, but in case you do, here are the instructions on doing so as written by dfannin in the original project. 
> I had to change the ublox firmware settings to produced the desired output using the Ublox Ucenter software (available from the Ublox website). You'll need to connect the tx/rx and gnd pins to a USB/TTL 5/3.3v FTDI or equivlent cable and connect to a computer running the Ucenter (windows based) software. I used Ubuntu/Wine to run the Ucenter software - it would crash sometimes, but I was to able to reprogram the module. I changed the following settings and saved them to the EEPROM: Output: NMEA, Output Version 2.3, Compatiblity Mode (checked), GPZDA messages set output to true.

### Notes on making an enclosure
This should definitely be put in an enclosure for field use so that the electronics aren't just dangling around. dfannin used a mint tin for theirs, you may want to use something that can clip onto your bag or belt. If you use a metal enclosure, the blocky GPS antenna connected to the module must be on the outside for it to get reception. 
