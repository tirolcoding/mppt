#include <Arduino.h>
#include <SoftwareSerial.h>
#include <hexlib.h>

#define RX_1_PIN 24
#define TX_1_PIN 23

void setup()
{
  Serial.begin(19200);
  Serial.println("hello world");
  Serial1.begin(19200);
  delay(100);
  Serial1.write(":451\n");

}

String label, val;
uint8_t buffer[100];


void loop()
{
  struct hexlib_frame frame;
  if(parseIncoming(Serial1, Serial, frame) == 1) {
    /* frame data buffer has the following structure (endianness is switched) */
    /* Example temperature reading:
      Receiving the fhe following command:
      :7DBED0042093B\n is a GET response to register 0xEDDB
      frame->cmd = 0x7 (GET)
      frame->data[0] = 3B (Checksum)
      frame->data[1] = data high nibble 0x09
      frame->data[2] = data low nibble 0x42
      value would be: 0x0942 -> 2370°C * 0.01 = 23,7°C
      frame->data[3] = 0x00
      frame->data[4] = 0xED
      frame->data[5] = 0xDB
    */
  }

  delay(500);

  sendGetCommand(CHARGER_INTERNAL_TEMP, Serial1);
  // sendGetCommand(0xEDF0, Serial1);
}
