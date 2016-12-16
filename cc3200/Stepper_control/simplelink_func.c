/*******************************************************************
   $File:    simplelink_func.c
   $Date:    Sat, 10 Dec 2016: 17:00
   $Version: 
   $Author:  klek 
   $Notes:   
********************************************************************/

// General
#include "simplelink_func.h"
#include "network_cred.h"
#include "movement.h"

// C-libraries
#include <string.h>

// Driverlib includes


// Common includes
#include "common.h"
#include "uart_if.h"

// Simplelink includes
#include "simplelink.h"

// HTTP client libraries
#include <http/client/httpcli.h>
#include <http/client/common.h>

/*
 * Static variables (globals in this scope)
 */
// NOTE(klek): Copied from example
static volatile unsigned long  g_ulStatus = 0;//SimpleLink Status
static unsigned long  g_ulDestinationIP; // IP address of destination server
static unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
static unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
static unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID
//static unsigned char g_buff[MAX_BUFF_SIZE+1];
//static long bytesReceived = 0; // variable to store the file size


// Event-handler for Simplelink
// NOTE(klek): Copied from examples
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(!pWlanEvent)
    {
        return;
    }

    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'-Applications
            // can use it if required
            //
            //  slWlanConnectAsyncResponse_t *pEventData = NULL;
            // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            //

            // Copy new connection SSID and BSSID to global parameters
            memcpy(g_ucConnectionSSID,pWlanEvent->EventData.
                   STAandP2PModeWlanConnected.ssid_name,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
            memcpy(g_ucConnectionBSSID,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                   SL_BSSID_LENGTH);

            UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s ,"
                        "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                      g_ucConnectionSSID,g_ucConnectionBSSID[0],
                      g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                      g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                      g_ucConnectionBSSID[5]);
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request,
            //'reason_code' is SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION
            if(SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                UART_PRINT("[WLAN EVENT]Device disconnected from the AP: %s,"
                "BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            else
            {
                UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s,"
                "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
            memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
        }
        break;

        case SL_WLAN_STA_CONNECTED_EVENT:
        {
            // when device is in AP mode and any client connects to device cc3xxx
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION_FAILED);

            //
            // Information about the connected client (like SSID, MAC etc) will
            // be available in 'slPeerInfoAsyncResponse_t' - Applications
            // can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.APModeStaConnected;
            //

        }
        break;

        case SL_WLAN_STA_DISCONNECTED_EVENT:
        {
            // when client disconnects from device (AP)
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_LEASED);

            //
            // Information about the connected client (like SSID, MAC etc) will
            // be available in 'slPeerInfoAsyncResponse_t' - Applications
            // can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.APModestaDisconnected;
            //
        }
        break;

        case SL_WLAN_SMART_CONFIG_COMPLETE_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_SMARTCONFIG_START);

            //
            // Information about the SmartConfig details (like Status, SSID,
            // Token etc) will be available in 'slSmartConfigStartAsyncResponse_t'
            // - Applications can use it if required
            //
            //  slSmartConfigStartAsyncResponse_t *pEventData = NULL;
            //  pEventData = &pSlWlanEvent->EventData.smartConfigStartResponse;
            //

        }
        break;

        case SL_WLAN_SMART_CONFIG_STOP_EVENT:
        {
            // SmartConfig operation finished
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_SMARTCONFIG_START);

            //
            // Information about the SmartConfig details (like Status, padding
            // etc) will be available in 'slSmartConfigStopAsyncResponse_t' -
            // Applications can use it if required
            //
            // slSmartConfigStopAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.smartConfigStopResponse;
            //
        }
        break;

        case SL_WLAN_P2P_DEV_FOUND_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_P2P_DEV_FOUND);

            //
            // Information about P2P config details (like Peer device name, own
            // SSID etc) will be available in 'slPeerInfoAsyncResponse_t' -
            // Applications can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.P2PModeDevFound;
            //
        }
        break;

        case SL_WLAN_P2P_NEG_REQ_RECEIVED_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_P2P_REQ_RECEIVED);

            //
            // Information about P2P Negotiation req details (like Peer device
            // name, own SSID etc) will be available in 'slPeerInfoAsyncResponse_t'
            //  - Applications can use it if required
            //
            // slPeerInfoAsyncResponse_t *pEventData = NULL;
            // pEventData = &pSlWlanEvent->EventData.P2PModeNegReqReceived;
            //
        }
        break;

        case SL_WLAN_CONNECTION_FAILED_EVENT:
        {
            // If device gets any connection failed event
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION_FAILED);
        }
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                       pWlanEvent->Event);
        }
        break;
    }
}

// Network actions are handled here such as IP acquisition/release etc
// NOTE(klek): Copied from examples
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            //Ip Acquired Event Data
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

            //Gateway IP address
            g_ulGatewayIP = pEventData->gateway;

            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
            "Gateway=%d.%d.%d.%d\n\r",
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,0),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,0));
        }
        break;

        default:
        {
            UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Event);
        }
        break;
    }
}

// Handles general events
// NOTE(klek): Copied from examples
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    if(!pDevEvent)
    {
        return;
    }

    //
    // Most of the general errors are not FATAL are are to be handled
    // appropriately by the application
    //
    UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->EventData.deviceEvent.status,
               pDevEvent->EventData.deviceEvent.sender);
}

// Handles socket events.
// NOTE(klek): Copied from examples
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    //
    // This application doesn't work w/ socket - Events are not expected
    //
    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status)
            {
                case SL_ECLOSE:
                    UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                                "failed to transmit all queued packets\n\n",
                                    pSock->socketAsyncEvent.SockTxFailData.sd);
                    break;
                default:
                    UART_PRINT("[SOCK ERROR] - TX FAILED  :  socket %d , reason "
                                "(%d) \n\n",
                                pSock->socketAsyncEvent.SockTxFailData.sd, pSock->socketAsyncEvent.SockTxFailData.status);
                  break;
            }
            break;

        default:
            UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);
          break;
    }

}

// Handles HTTP-server events
// NOTE(klek): Copied from examples
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    // Unused in this application
}

// Configures simplelink to the default state, which is
//     - Set the mode to STATION
//     - Configures connection policy to Auto and AutoSmartConfig
//     - Deletes all the stored profiles
//     - Enables DHCP
//     - Disables Scan policy
//     - Sets Tx power to maximum
//     - Sets power policy to normal
//     - Unregister mDNS services
//     - Remove all filters
// NOTE(klek): Copied from examples
static long ConfigureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);

    // If the device is not in station-mode, try configuring it in station-mode 
    if (ROLE_STA != lMode)
    {
        if (ROLE_AP == lMode)
        {
            // If the device is in AP mode, we need to wait for this event 
            // before doing anything 
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
            }
        }

        // Switch to STA role and restart 
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again 
        if (ROLE_STA != lRetVal)
        {
            // We don't want to proceed if the device is not coming up in STA-mode 
            return DEVICE_NOT_IN_STATION_MODE;
        }
    }
    
    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt, 
                                &ucConfigLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(lRetVal);
    
    UART_PRINT("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

    // Set connection policy to Auto + SmartConfig 
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, 
                                SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove all profiles
    lRetVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(lRetVal);

    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore 
    // other return-codes
    //
    lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal)
    {
        // Wait
        while(IS_CONNECTED(g_ulStatus))
        {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask(); 
#endif
        }
    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, 
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);


    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(lRetVal);

    g_ulStatus = 0;
    g_ulGatewayIP = 0;
    memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));

    return SUCCESS;
}

// Connects to the network specified by the macros
// NOTE(klek): Copied from examples
// NOTE(klek): Modified with network credentials
long wlanConnect()
{
    SlSecParams_t secParams = {0};
    long lRetVal = 0;

    secParams.Key = (signed char *)NETWORK_SEC_KEY;
    secParams.KeyLen = strlen(NETWORK_SEC_KEY);
    secParams.Type = NETWORK_SEC_TYPE;

    lRetVal = sl_WlanConnect((signed char *)NETWORK_SSID_NAME,
                           strlen((const char *)NETWORK_SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Wait for WLAN Event
    while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus)))
    {
        // wait till connects to an AP
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask();
#endif
    }

    Report("Connected to %s \n\r", g_ucConnectionSSID);
    
    return SUCCESS;
}

// Configures simplelink to default state and starts the wlan
long wlanStart(void)
{
    long retVal = -1;

    retVal = ConfigureSimpleLinkToDefaultState();
    if ( retVal < 0 ) {
        if ( retVal == DEVICE_NOT_IN_STATION_MODE ) {
            Report("Failed to configure the device in its default state. Error %d \n\r", retVal);
        }
        while(1);
    }

    // Device is configured in default state
    Report("Device configured in default state as station\n\r");

    // Start the wlan
    retVal = sl_Start(0, 0, 0);
    if ( retVal < 0 ) {
        ASSERT_ON_ERROR(DEVICE_START_FAILED);
    }

    // Device started as station
    Report("Device started as station\n\r");

    return SUCCESS;
}

// Configures the HTTP connection with valid data and tests it
long setupHTTPConnection(HTTPCli_Struct* cli, struct sockaddr_in* adr)
{
    // Initialize variables
    long retVal = -1;

    retVal = sl_NetAppDnsGetHostByName((signed char *)HOST_NAME,
                                       strlen((const char *)HOST_NAME),
                                       &g_ulDestinationIP, SL_AF_INET);
    if ( retVal < 0 ) {
        Report("Getting host ip failed\n\r");
    }

    // Setup the socket address
    adr->sin_family = AF_INET;
    adr->sin_port = htons(HOST_PORT);
    adr->sin_addr.s_addr = sl_Htonl(g_ulDestinationIP);

    // Test the connection
    HTTPCli_construct(cli);
    retVal = HTTPCli_connect(cli, (struct sockaddr *)adr, 0, NULL);
    if ( retVal < 0 ) {
        Report("Connection to server failed\n\r");
    }
    else {
        Report("Connection to server created successfully\n\r");
    }

    // Lets return success
    return SUCCESS;    
}

// Fetches data through HTTP-request. After that it parses the data
// and puts it in the specified array
// NOTE(klek); Perhaps second argument should simply be a char buffer??
long fetchAndParseData(HTTPCli_Struct* cli, unsigned char* buff/*struct order* buff*/, unsigned int size)
{
    // Start by checking if size is valid
    if ( size < 1 ) {
        Report("Buffer size is invalid\n\r");
        return FAILURE;
    }
    else {
        // Setup variables
        long retVal = -1;
        int id = 0;
        bool moreData = 0;
        char httpBuff[MAX_BUFF_SIZE + 1];
        unsigned int orderIndex = 0;

        // Setup HTTP-fields
        HTTPCli_Field fields[3] = {
            {HTTPCli_FIELD_NAME_HOST, HOST_NAME},
            {HTTPCli_FIELD_NAME_ACCEPT, "text/html, application/xhtml+xml, */*"},
            {NULL, NULL}                              
        };

        // Setup ids
        const char* ids[4] = {
                            HTTPCli_FIELD_NAME_CONTENT_LENGTH,
                            HTTPCli_FIELD_NAME_TRANSFER_ENCODING,
                            HTTPCli_FIELD_NAME_CONNECTION,
                            NULL
        };

        // Set request fields
        HTTPCli_setRequestFields(cli, fields);

        // memset the array to 0??

        // Make the request
        retVal = HTTPCli_sendRequest(cli, HTTPCli_METHOD_GET, HOST_LOC_1, 0);
        if ( retVal < 0 ) {
            Report("TCP-send error");
            return FAILURE;
        }

        // Test the response
        retVal = HTTPCli_getResponseStatus(cli);
        if ( retVal != 200 ) {
            flushHTTPResponse(cli);
            if ( retVal == 404 ) {
                Report("File not found\n\r");
                return FAILURE;
            }
            return FAILURE;
        }

        // Apparently response was good
        // Request more
        HTTPCli_setResponseFields(cli, ids);

        // Read the response headers
        while ( ( id = HTTPCli_getResponseField(cli, (char *)httpBuff, sizeof(httpBuff), &moreData))
                != HTTPCli_FIELD_ID_END )
        {
            if ( id == 0 ) {

            }
            else if ( id == 1 ) {
                if(!strncmp((const char *)httpBuff, "chunked", sizeof("chunked")))
                {
                    Report("Chunked transfer encoding\n\r");
                }

            }
            else if ( id == 2 ) {
                if ( !strncmp((const char*)httpBuff, "close", sizeof("close")) ){
                    Report("Unsupported file-format\n\r");
                    return FAILURE;
                }
            }
        }

        // Fetch the data
        // TODO(klek): Fix this loop
        moreData = true;
        while ( moreData ) {
            // Request data
            retVal = HTTPCli_readResponseBody(cli, (char *)httpBuff, sizeof(httpBuff) - 1, &moreData);

            // Checked if we got any data
            if ( retVal < 0 ) {
                Report("File was empty\n\r");
                return FAILURE;
            }

            // Debugging purpose
            Report("Data recieved: %s\n\r", httpBuff);

            // Parse the data and put it into our order array
            // How do we find the first command?
            int i = 0;
            char valid = 1;
            char command = 0;
            for ( ; i < retVal && orderIndex < size ; ) {
                // Assume valid data is now present
                valid = 1;
                // Search for known commands
                switch(httpBuff[i]) {
                    case 'U':
                        // We have a command to move without drawing
                        // find the coordinates
                        // Set first byte this write to 0 to indicate Pen is up
                        //*(buff + (orderIndex++)) = PEN_UP;
                        command = PEN_UP;
                        break;

                    case 'D':
                        // We have a command to move as we are drawing
                        // find the coordinates
                        // Set first byte this write to 1 to indicate Pen is down
                        //*(buff + (orderIndex++)) = PEN_DOWN;
                        command = PEN_DOWN;
                        break;

                    case 'E':
                        // End of order, should read how many orders that
                        // should have been processed and then exit this loop
                        //*(buff + (orderIndex++)) = END_OF_DATA;
                        command = END_OF_DATA;
                        break;

                    case 'O':
                        // Move to origin, shouldn't be needed??
                        // Currently no valid data
                        valid = 0;
                        break;

                    case '\n':
                        // We found an endline sign
                        // No valid data
                        valid = 0;
                        break;
                        
                    default:
                        // No valid command was found, increment counter
                        valid = 0;
                        break;
                }

                // Copy data if we found a valid command
                if ( valid == 1 ) {
                    unsigned char t1[COORD_SIZE];
                    int j = 0;
                    for ( ; j < (COORD_SIZE) ; j++ ) {
                        t1[j] = httpBuff[++i];
                    }
                    // Increment the i-variable one more time
                    i++;
                    // Reuse the j variable as return value
                    j = charToInt(t1, COORD_SIZE * 2);
                    // Repack the data in j
                    j = packData(command, j);
                    // Save the third byte of j
                    *(buff + (orderIndex++)) = ((j & 0x00FF0000) >> 16);
                    // Save the second byte of j 
                    *(buff + (orderIndex++)) = ((j & 0x0000FF00) >> 8);
                    //Report("Upper + lower byte for X: %i \n\r", (*(buff + (orderIndex - 1)) + *(buff + (orderIndex - 2)) ));
                    // Save the first byte of j
                    *(buff + (orderIndex++)) = (j & 0x000000FF);
                    //Report("Upper + lower byte for Y: %i \n\r", (*(buff + (orderIndex - 1)) + *(buff + (orderIndex - 2)) ));
                }
                else {
                    // Increment i
                    i++;
                }
            }
        }
        // If we reach here we should simply return how many items we did put in our array
        return orderIndex;
    }
}

// Flushes the HTTP response
// NOTE(klek): Copied from examples
static int flushHTTPResponse(HTTPCli_Handle cli)
{
    const char *ids[2] = {
                            HTTPCli_FIELD_NAME_CONNECTION, /* App will get connection header value. all others will skip by lib */
                            NULL
                         };
    char  buf[128];
    int id;
    int len = 1;
    bool moreFlag = 0;
    char ** prevRespFilelds = NULL;

                            
    prevRespFilelds = HTTPCli_setResponseFields(cli, ids);

    // Read response headers
    while ((id = HTTPCli_getResponseField(cli, buf, sizeof(buf), &moreFlag))
            != HTTPCli_FIELD_ID_END)
    {
        if(id == 0)
        {
            if(!strncmp(buf, "close", sizeof("close")))
            {
                UART_PRINT("Connection terminated by server\n\r");
            }
        }
    }

    HTTPCli_setResponseFields(cli, (const char **)prevRespFilelds);

    while(1)
    {
        len = HTTPCli_readResponseBody(cli, buf, sizeof(buf) - 1, &moreFlag);
        ASSERT_ON_ERROR(len);

        if ((len - 2) >= 0 && buf[len - 2] == '\r' && buf [len - 1] == '\n')
        {
            break;
        }

        if(!moreFlag)
        {
            break;
        }
    }
    return SUCCESS;
}


// Converts chars to integers.
// Used to decode the ascii code recieved from HTTP request into
// orders performed by the CNC
// NOTE(klek): This function should be followed by packData to reduce
// the size of the order to 3 bytes
static unsigned int charToInt(unsigned char* temp, unsigned char size)
{
    int i;
    short int t1 = 0;
    int t2 = 0;
    for(i = 0; i < size; i++)
    {
        if ( ( *(temp + i) < 58 ) && ( *(temp + i) > 47 ) )
        {
            *(temp + i) = *(temp + i) - 48;
        }
        else
        {
            // Error message? Set the slot to zero
            *(temp + i) = 0;
        }
    }

    // Decode the message coordinates to ints
    //*xCoord = 100*temp[0]+10*temp[1]+temp[2];
    //*yCoord = 100*temp[3]+10*temp[4]+temp[5];

    t1 = (*(temp + 0) * 100) + (*(temp + 1) * 10) + *(temp + 2);
    t2 = (*(temp + 3) * 100) + (*(temp + 4) * 10) + *(temp + 5);

    
    // Return X-coordinate in upper 16 bytes
    // and Y-coordinate in lower 16 bytes
    return ((t1 << 16) + t2);
}

// Packs the data into the 3 lower bytes of the return value
static unsigned int packData(unsigned char command, unsigned int data)
{
    unsigned int t1 = 0;
    // Data contains X-koord in 10 bits of the upper 2 bytes
    // Downshift this data by 6 bits
    t1 = (data & X_COORD_CODE_MASK) >> X_COORD_DOWN_SHIFT;
    // t1 should now contain valid data in bits 10 - 19 = 10 bits
    // The lower 2 bytes is the Y-koord and should contain valid data in the
    // 10 lowest bytes only mask these bits and add it to t1 to get a new data
    data = t1 + (data & Y_COORD_CODE_MASK);

    // The data value should now contain the lowest 20 bits of data
    // Lets add the 4 lower bits of command to the 4 remaining bits of
    // the third byte
    data = ((command & 0x0F) << COMMAND_SHIFT) + data;

    return data;
}

// Unpacks 3 bytes of data from an array and returns them through reference into the
// provided struct.
// NOTE(klek): It is very important to provide an array that HAS boundaries outside
// of the scope of this function
unsigned short int unPackData(unsigned char* array, struct order* data)
{
    // The specified array contains data that we want to decipher
    // This function in particular deciphers 3 bytes of data into a struct order

    // Loop through the array in 3 steps and gather the data into an int
    //int i = 0;
    unsigned int t1 = 0;
    t1 = (*(array) << 16) + (*(array + 1) << 8) + *(array + 2);
    
    // t1 should now contain the data from 3 bytes
    // This data should be sorted the form
    // 00000000 CCCCXXXX XXXXXXYY YYYYYYYY
    // Where C = command
    //       X = X-coordinate
    //       Y = Y-coordinate
    // We need to read this data out and store it in the specified struct
    // Get the command
    switch ((t1 & COMMAND_DECODE_MASK) >> COMMAND_SHIFT) {
        case PEN_UP:
            data->pen = 'U';
            break;
        case PEN_DOWN:
            data->pen = 'D';
            break;
        case END_OF_DATA:
            data->pen = 'E';
        default:
            break;
    }
    data->xCoord = (t1 & X_COORD_DECODE_MASK) >> X_COORD_UP_SHIFT;
    data->yCoord = (t1 & Y_COORD_DECODE_MASK);

    // The struct data should now contain the valid data
    return PACKED_ORDER_SIZE;
}
