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

    // hexlib_frame *frame = (struct hexlib_frame *)malloc(sizeof(struct hexlib_frame));
    // memset(frame->buffer, 0 ,FRAME_SIZE);

    /* allocate buffer */
    
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
        snprintf(str, MAX_STRING_SIZE, "data[%d]: 0x%x ", i, frame.buffer[i]);
        Serial.print(str);
    }
    Serial.println("\r\n-----------------------------------");
}
