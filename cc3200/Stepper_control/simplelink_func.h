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


/*
 * Macros
 */
#define PREFIX_BUFFER      40960            // 40 kB file size
#define HOST_NAME          "www.klek.se"
#define HOST_PORT          80
#define READ_SIZE          1450             // Bytes to read at each time
#define MAX_BUFF_SIZE      100000//4096//1460             // Size of the buffer we use to middle-store data
#define FILE_NAME          ""               // TODO(klek): Choose an appropriate file name.
                                            // Maybe one that is changeble rather than a macro?

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


#endif
