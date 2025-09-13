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
  parseIncoming(Serial1, Serial);

  delay(500);

  sendGetCommand(CHARGER_INTERNAL_TEMP, Serial1);
  // sendGetCommand(0xEDF0, Serial1);
}
