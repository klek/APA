/*******************************************************************
   $File:    simplelink_func.h
   $Date:    Sat, 10 Dec 2016: 17:00
   $Version: 
   $Author:  klek 
   $Notes:   
********************************************************************/

#if !defined(SIMPLELINK_FUNC_H)
#define SIMPLELINK_FUNC_H

// Simplelink includes
#include "simplelink.h"

// HTTP client libraries
#include <http/client/httpcli.h>
#include <http/client/common.h>

// Other
#include "movement.h"

/*
 * Macros
 */
#define HOST_NAME            "www.klek.se"
#define HOST_PORT            80
#define HOST_LOC_1           "/downloads/part01.txt"
#define HOST_LOC_2           "/downloads/part02.txt"

#define FILE_SIZE            40960            // 40 kB file size
#define READ_SIZE            1450             // Bytes to read at each time
#define MAX_BUFF_SIZE        1460             // Size of the buffer we use to middle-store data
#define FILE_NAME            ""               // TODO(klek): Choose an appropriate file name.
                                              // Maybe one that is changeble rather than a macro?

// Defines regarding how data is packed
#define PACKED_ORDER_SIZE    3                // We want to save data in chunks of 3 bytes
#define COMMAND_SHIFT        20               // The command should be shifted with 20 bits
#define X_COORD_UP_SHIFT     10               // The X-coord should be shifted with 10 bits
#define Y_COORD_UP_SHIFT     0                // The Y-coord should be shifted with 0 bits
#define X_COORD_DOWN_SHIFT   6                // The X-coord should be downshifted with 6 bits
#define X_COORD_CODE_MASK    0x03FF0000       // Masks the correct bits for coding of X-coord
#define Y_COORD_CODE_MASK    0x000003FF       // Masks the correct bits for coding of Y-coord
#define X_COORD_DECODE_MASK  0x000FFC00       // Masks the correct bits for decoding of X-coord
#define Y_COORD_DECODE_MASK  0x000003FF       // Masks the correct bits for decoding of Y-coord
#define COMMAND_DECODE_MASK  0x00F00000       // Masks the correct bits for decoding of the command

/*
 * Structs
 */
// NOTE(klek): Copied from examples
typedef enum{
 /* Choosing this number to avoid overlap with host-driver's error codes */
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,       
    DEVICE_START_FAILED = DEVICE_NOT_IN_STATION_MODE - 1,
    INVALID_HEX_STRING = DEVICE_START_FAILED - 1,
    TCP_RECV_ERROR = INVALID_HEX_STRING - 1,
    TCP_SEND_ERROR = TCP_RECV_ERROR - 1,
    FILE_NOT_FOUND_ERROR = TCP_SEND_ERROR - 1,
    INVALID_SERVER_RESPONSE = FILE_NOT_FOUND_ERROR - 1,
    FORMAT_NOT_SUPPORTED = INVALID_SERVER_RESPONSE - 1,
    FILE_OPEN_FAILED = FORMAT_NOT_SUPPORTED - 1,
    FILE_WRITE_ERROR = FILE_OPEN_FAILED - 1,
    INVALID_FILE = FILE_WRITE_ERROR - 1,
    SERVER_CONNECTION_FAILED = INVALID_FILE - 1,
    GET_HOST_IP_FAILED = SERVER_CONNECTION_FAILED  - 1,
    
    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;


// Event-handler for Simplelink
// NOTE(klek): Copied from examples
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent);


// Network actions are handled here such as IP acquisition/release etc
// NOTE(klek): Copied from examples
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent);


// Handles general events
// NOTE(klek): Copied from examples
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent);


// Handles socket events.
// NOTE(klek): Copied from examples
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock);


// Handles HTTP-server events
// NOTE(klek): Copied from examples
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse);

// Configures simplelink to default state
// NOTE(klek): Copied from examples
static long ConfigureSimpleLinkToDefaultState();

// Connects to the network specified by the macros
// NOTE(klek): Copied from examples
// NOTE(klek): Modified with network credentials
long wlanConnect();

// Configures simplelink to default state and starts the wlan
long wlanStart(void);

// Configures the HTTP connection with valid data and tests it
long setupHTTPConnection(HTTPCli_Struct* cli, struct sockaddr_in* adr);

// Fetches data through HTTP-request. After that it parses the data
// and puts it in the specified array
long fetchAndParseData(HTTPCli_Struct* cli, unsigned char* buff/*struct order* buff*/, unsigned int size);
//long fetchAndParseData(HTTPCli_Struct* cli, struct order* buff, unsigned int size);

// Flushes the HTTP response
// NOTE(klek): Copied from examples
static int flushHTTPResponse(HTTPCli_Handle cli);

// Converts chars to integers.
// Used to decode the ascii code recieved from HTTP request into
// orders performed by the CNC
static unsigned int charToInt(unsigned char* temp, unsigned char size);

// Packs the data into the 3 lower bytes of the return value
static unsigned int packData(unsigned char command, unsigned int data);

// Unpaks the data into the 3 lower bytes of the return value
void unPackData(unsigned int data, struct order* retVal);



#endif
