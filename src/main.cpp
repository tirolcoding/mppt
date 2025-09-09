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
  /* read response */
  while (Serial1.available())
  {
    /* read them line wise */
    String raw = Serial1.readStringUntil('\n');
    int marker = raw.indexOf(":");
    if(marker == 0) {
      /* scan for the command */
      char cmd = raw.charAt(marker + 1);
      switch(cmd){
        case '0':
        case '1':
        case '3':
        case '4':
        case '5':
        case '7':
        case '8': {
          Serial.print("command is:");
          Serial.println(raw);
          struct hexlib_frame frame;
          int ret = str_to_frame(frame, raw);
          if (ret != -1) {
            print_frame(frame);
            // free(frame);
          }
        }
          break;
        case 'A':
        /* we can ignore the async responses */
        default:
          break;
      }

      //Serial.println(raw);
    }
  }

  delay(500);

  sendGetCommand(CHARGER_INTERNAL_TEMP, Serial1);
  // sendGetCommand(0xEDF0, Serial1);
}
