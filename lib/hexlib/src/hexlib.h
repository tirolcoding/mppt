/**
 * Library for SmartSolar MPPT
 * 
 * 
 * 2025
 */

#include <inttypes.h>
#include <WString.h>
#include <HardwareSerial.h>

#ifndef HEXLIB_H_
#define HEXLIB_H_

/**
 * HEX PROTOCOL defines listed in VE.Direct protocol
 * : [command] [data][data][…] [check]\n
 * : [command] [dataHighNibble, dataLowNibble][……] [checkHigh, checkLow] \n
 * 
 */
#define COMMAND_ENTER_BOOT 0x1
#define COMMAND_PING 0x2
#define COMMAND_APP_VERSION 0x3
#define COMMAND_PRODUCT_ID 0x4
#define COMMAND_RESTART 0x6
#define COMMAND_GET 0x7
#define COMMAND_SET 0x8
#define COMMAND_ASYNC 0xA

#define RESPONSE_DONE 0x1
#define RESPONSE_UNKNOWN 0x3
#define RESPONSE_ERROR 0x4
#define RESPONSE_PING 0x5
#define RESPONSE_GET 0x7
#define RESPONSE_SET 0x8

#define FLAG_UNKNOWN_ID 0x1
#define FLAG_NOT_SUPPORTED 0x2
#define FLAG_PARAMETER_ERROR 0x4

/* registers */
#define CHARGER_INTERNAL_TEMP 0xEDDB

/* some global defines */
#define FRAME_SIZE 50
#define CHECKSUM_TOTAL 0x55

struct hexlib_frame {
    /* value containing the command */
    uint8_t cmd;
    /* number of received data */
    uint8_t num_bytes;
    /* pointer of FRAME_SIZE containing data */
    uint8_t buffer[FRAME_SIZE];
    /* calculated checksum */
    uint8_t check;
    /* expected checksum from received frame */
    uint8_t expected_check;
};

/**
 * @brief This function takes a raw string from serial port and converts it to a hexlib_frame struct.
 * 
 * @param str String parameter
 */
int str_to_frame(hexlib_frame &frame, String str);

/**
 * @brief This function takes a hexlib_frame and converts it to a raw string which can be send via serial port.
 */
String frame_to_str(struct hexlib_frame);

/**
 * @brief Sends a GET command to the mppt device
 * 
 * @param id id of the register to read
 * @param serial a HardwareSrial interface physically connencted
 */
void sendGetCommand(uint16_t id, HardwareSerial &serial);


/**
 * @brief Function to parse the stream after a command was sent and print the output to the console
 * 
 * 
 */
void parseIncoming(HardwareSerial &serial, HardwareSerial &console);


void print_frame(struct hexlib_frame &frame);

#endif // HEXLIB_H_
