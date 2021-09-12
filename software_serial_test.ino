#include <Arduino.h>
#include "wiring_private.h"

int pin1 = 5; //these are here to help me try to brute force pins faster
int pin2 = 6;

Uart mySerial (&sercom0, pin1, pin2, SERCOM_RX_PAD_1, UART_TX_PAD_0);

// Attach the interrupt handler to the SERCOM
void SERCOM0_Handler()
{
    mySerial.IrqHandler();
}

void setup() {
  // Reassign pins 5 and 6 to SERCOM alt
  //temporarily changing 5 and 6 to 11 and 12
  pinPeripheral(pin1, PIO_SERCOM_ALT);
  pinPeripheral(pin2, PIO_SERCOM_ALT);

  // Start my new hardware serial
  mySerial.begin(9600);
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }

  if (Serial1.available()) {
    Serial.write(Serial1.read());
  }
}
