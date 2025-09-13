#include <hexlib.h>
#include <Arduino.h>

#define MAX_STRING_SIZE 20

/* Function expects following substring: ':[cmd][data]..[data][check]\n\0' */
int str_to_frame(hexlib_frame &frame, String str) {
    int end = str.lastIndexOf('\0');
    int start = str.lastIndexOf(':');
    if(start < 0 || end < 0) {
        /* invalid string */
        Serial.println("invalid string");
        return -1;
    }

    if(str.length() > FRAME_SIZE)
        return -1;

    /* remove the ':' and cmd */
    String data = str.substring(start + 2, str.indexOf('\n'));
    int num_bytes;
    if(data.length() % 2 != 0) {
        num_bytes = (data.length() / 2) + 1;
    } else {
        num_bytes = data.length() / 2;
    }

    /* parse the cmd */
    char cmd[2];
    cmd[0] = str.charAt(start + 1);
    cmd[1] = 0;
    frame.cmd = strtoul(cmd, NULL, 16);

    uint8_t buffer[FRAME_SIZE];
    uint8_t checksum = CHECKSUM_TOTAL - frame.cmd;
    for(uint8_t i = 0; i < num_bytes; i++) {
        char tmp[3];
        tmp[0] = data[(i) *2];
        tmp[1] = data[((i) *2) + 1];
        tmp[2] = '\0';
        buffer[i] = strtoul(tmp, NULL, 16);
        if(i != (num_bytes -1))
            checksum -= buffer[i];
    }

    frame.check = checksum;
    frame.expected_check = buffer[num_bytes -1];
    frame.num_bytes = num_bytes;

    /* fill the frame buffer with switched endianness */
    for(uint8_t i = 0; i < num_bytes; i++) {
        frame.buffer[i] = buffer[(num_bytes -1) - i];
    }

    return 0;
}

void print_frame(struct hexlib_frame &frame) {
    Serial.println("RECEIVED FRAME ...");
    Serial.print("Checksum: 0x");
    Serial.print(frame.check, 16);
    Serial.print("\r\nExcpected checksum: 0x");
    Serial.print(frame.expected_check, 16);
    Serial.print("\r\nCommand: 0x");
    Serial.print(frame.cmd, 16);
    Serial.println();
    for(int i = 0; i < frame.num_bytes; i++) {
        char str[MAX_STRING_SIZE];
        snprintf(str, MAX_STRING_SIZE, "data[%d]: 0x%02X ", i, frame.buffer[i]);
        Serial.print(str);
    }
    Serial.println("\r\n-----------------------------------");
}

void sendGetCommand(uint16_t id, HardwareSerial &serial) {
    uint16_t reg = __builtin_bswap16(id);
    uint8_t checksum = CHECKSUM_TOTAL;
    checksum -= 0x7;
    checksum -=  (reg >> 8);
    checksum -= (reg & 0x00FF);
    char temp[12];
    snprintf(temp, sizeof(temp), ":7%04X00%02X\n", reg, checksum);
    serial.write(temp);
}

static void printFrame(HardwareSerial &console, struct hexlib_frame &frame) {
    console.println("RECEIVED FRAME");
    console.print("Checksum: 0x");
    console.print(frame.check, 16);
    console.print("\r\nCommand: 0x");
    console.print(frame.cmd, 16);
    console.println();
    for(int i = 0; i < frame.num_bytes; i++) {
        char str[MAX_STRING_SIZE];
        snprintf(str, MAX_STRING_SIZE, "data[%d]: 0x%02X ", i, frame.buffer[i]);
        console.print(str);
    }
    console.println("\r\n-----------------------------------");
}

static bool isValidResponse(String line) {
    return (line.indexOf(":") == 0);
}

// function is blocking so be careful
void parseIncoming(HardwareSerial &serial, HardwareSerial &console) {
    /* read input in lines */
    while (serial.available()) {
        String line = serial.readStringUntil('\n');
        if(isValidResponse(line)) {
            /* scan for the command */
            switch(line.charAt(1)){
                /* cmd successful */
                case '1':
                /* cmd unknown */
                case '3':
                /* cmd error */
                case '4':
                /* cmd ping */
                case '5':
                /* cmd get */
                case '7':
                /* cmd set */
                case '8': {
                struct hexlib_frame frame;
                int ret = str_to_frame(frame, line);
                if (ret != -1) {
                    print_frame(frame);
                    // free(frame);
                }
                }
                break;
            }
        }
    }
}
