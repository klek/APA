/*******************************************************************
   $File:    main_new.c
   $Date:    Fri, 09 Dec 2016: 19:14
   $Version: 
   $Author:  klek 
   $Notes:   
********************************************************************/

// Standard includes
#include <stdio.h>
#include <stdlib.h>

// Driverlib includes
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "rom_map.h"
#include "uart.h"
#include "gpio.h"
#include "interrupt.h"
#include "prcm.h"

// Simplelink includes
//#include "simplelink_func.h"
#include "simplelink.h"
#include "netcfg.h"

// Free rtos / TI-RTOS includes
#include "osi.h"

// Common includes
#include "uart_if.h"
#include "common.h"

// Special headers
#include "movement.h"
#include "gpio_func.h"

/*
 * Macros
 */
#define WORK_TASK_STACK_SIZE      (2048)
#define DATA_TASK_STACK_SIZE      (2048)
#define INIT_TASK_STACK_SIZE      (512)
#define WORK_TASK_PRIORITY        (3)
#define DATA_TASK_PRIORITY        (1)
#define INIT_TASK_PRIORITY        (2)

#define INTERRUPT_PRIORITY        10

// Macros for bits in status vector
#define X_HOME                    (1 << 0)
#define Y_HOME                    (1 << 1)
#define HOME_FOUND                (1 << 2)

// Macros for message queue
#define MAX_MSG_LENGTH            8
#define MAX_Q_LENGTH              5

/*
 * Global variables
 */
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif

/*
 * Static variables (global)
 */
// NOTE(klek): Maybe this should be in a separate file?
static struct position myPos;
// Status variable to indicate interrupts
// NOTE(klek): Maybe increase this one to keep track of when to get more data from web?
static unsigned char status;

static OsiMsgQ_t workMsg;
static OsiMsgQ_t dataMsg;

/*
 * External stuff
 */
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

// C-libraries
#include <string.h>

// Driverlib includes


// Common includes
#include "common.h"
#include "uart_if.h"

// Simplelink includes
#include "simplelink.h"

// HTTP client libraries
//#include <http/client/httpcli.h>
//#include <http/client/common.h>

/*
 * Static variables (globals in this scope)
 */
// NOTE(klek): Copied from example
static volatile unsigned long  g_ulStatus = 0;//SimpleLink Status
static unsigned long  g_ulDestinationIP; // IP address of destination server
static unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
static unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
static unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID
static unsigned char g_buff[MAX_BUFF_SIZE+1];
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


/*
 * Function prototypes
 */
// Interrupt handler for homing-switches
static void xAxisHandler(void);
static void yAxisHandler(void);

// Initialize the components of the board
static void boardInit(void);

// Function to display banner on UART at startup
static void displayBanner(void);

// Task to initialize
static void initTask(void* params);

// This task is supposed to do all the work
static void workTask(void* params);

// This task should gather data for the workTask
static void dataTask(void* params);

/*
 * The main function
 */
void main(void)
{
    // Set the status vector to zero
    status = 0;
    
    // Return value to be used for error checking
    long retVal = -1;

    // Initialize the board
    boardInit();

    // Setup the pins
    pinMuxConfig();

    // Initialize uart
    MAP_PRCMPeripheralReset(PRCM_UARTA0);
    MAP_UARTConfigSetExpClk(CONSOLE, MAP_PRCMPeripheralClockGet(CONSOLE_PERIPH), UART_BAUD_RATE,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Display banner and info in the UART-console
    displayBanner();

    // Setup simplelink
    retVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if ( retVal < 0 ) {
        Report("Unable to start simplelink spawn task\n\r");
        while(1);
    }

    /*
     * Message queue for communication between tasks
     */
    // The message queue for data to workTask
    retVal = osi_MsgQCreate(&workMsg, "workMsg", MAX_MSG_LENGTH, MAX_Q_LENGTH);
    if ( retVal < 0 ) {
        Report("Failed to create queue for data to workTask\n\r");
        while(1);
    }
    
    /*
     * Create interrupts
     */
    // Create the interrupt for HOME_X_AXIS
    retVal = osi_InterruptRegister(gpioGetIntBase(HOME_X_AXIS), xAxisHandler, INTERRUPT_PRIORITY);
    if ( retVal < 0 ) {
        Report("Failed to register the interrupts\n\r");
        while(1);
    }

    // Create the interrupt for HOME_Y_AXIS
    retVal = osi_InterruptRegister(gpioGetIntBase(HOME_Y_AXIS), yAxisHandler, INTERRUPT_PRIORITY);
    if ( retVal < 0 ) {
        Report("Failed to register the interrupts\n\r");
        while(1);
    }
    // Enable both interrupts
    gpioEnableInterrupts(HOME_X_AXIS);
    gpioEnableInterrupts(HOME_Y_AXIS);
    
    /*
     * Create tasks
     */
    // Create init task
    retVal = osi_TaskCreate(initTask, (signed char*)"initTask",
                            INIT_TASK_STACK_SIZE, NULL, INIT_TASK_PRIORITY, NULL);
    if ( retVal < 0 ) {
        Report("Failed to create initTask\n\r");
        while(1);
    }
    
    // Create the work task
    retVal = osi_TaskCreate(workTask, (signed char*)"workTask",
                            WORK_TASK_STACK_SIZE, NULL, WORK_TASK_PRIORITY, NULL);
    if ( retVal < 0 ) {
        Report("Failed to create workTask\n\r");
        while(1);
    }

    // Create the data collection task
    retVal = osi_TaskCreate(dataTask, (signed char*)"dataTask",
                            DATA_TASK_STACK_SIZE, NULL, DATA_TASK_PRIORITY, NULL);
    if ( retVal < 0 ) {
        Report("Failed to create dataTask\n\r");
        while(1);
    }


    // Start the OS scheduler
    osi_start();

    // We should never reach this :S
    while(1);
}

// Initialize the components of the board
static void boardInit(void)
{
    // Setup for te vector table
    // If TR-RTOS would be used, the OS itself would do this
#if !defined(USE_TIRTOS)
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#endif

    // Enable the processor
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}


// Displays a banner
static void displayBanner(void)
{
    Report("\n\n\n\r");
    Report("\t\t ********************************\n\r");
    Report("\t\t\t APA cc3200 application \n\r");
    Report("\t\t ********************************\n\r");
    Report("\n\n\n\r");
}

// Interrupt handler for x-axis switch
static void xAxisHandler(void)
{
    // Get which pins gave the interrupt
    unsigned long pinState = GPIOIntStatus(GPIOA2_BASE, 1);

    // Check which pin gave interrupt
    // NOTE(klek): This doesn't seem to work??
//    if ( pinState & HOME_X_AXIS ) {
//        while(1);
//    }

    // Set the corresponding bit in status vector
    status |= X_HOME;

    // Clear the interrupt
    gpioClearInt(HOME_X_AXIS);
}

// Interrupt handler for y-axis switch
static void yAxisHandler(void)
{
    // Get which pins gave the interrupt
    unsigned long pinState = GPIOIntStatus(GPIOA3_BASE, 1);

    // Check which pin gave interrupt
//    if ( !(pinState & HOME_Y_AXIS) ) {
//        // Wrong interrupt, lock us for now
//        while(1);
//    }

    // Set the corresponding bit in status vector
    status |= Y_HOME;

    // Clear the interrupt
    gpioClearInt(HOME_Y_AXIS);
}

// This task should move printer head to homing position
// Then it should tell workTask about this (through message queue)
// TODO(klek): Call this again when print is finished to calculate
//             how much offset we have gained during the print
static void initTask(void* params)
{
    // We should start by moving to homing position
    // to be able to set our position to zero
    // TODO(klek): Implement this function instead of loops
    //goToPos(HOME, &myPos);

    // Start by going to X_HOME
    // Wait for status-flag X_HOME to come true in status reg
    while ( !(status & X_HOME) ) {
        move(NEG_X);
    }
    // We should now be at zero x-position
    myPos.xPosition = 0;

    // Follow up by going to Y_HOME
    while ( !(status & Y_HOME) ) {
        move(NEG_Y);
    }
    // We should now be at zero y-position
    myPos.yPosition = 0;

    // Print out in console
    Report("Home position found\n\r");
    
    // Indicate for workTask that we can start working
    status |= HOME_FOUND;

    while (1) {
        osi_Sleep(100);
    }
}


// Task to carry out all the orders found in the databuffer
static void workTask(void* params)
{
    // Task setup
 
    // Wait for home position to be found
    while ( !(status & HOME_FOUND) ) {
        osi_Sleep(100);
    }

    // What should we do here?
    // TODO(klek): We should have a databuffer with orders
    // looking that contain which coordinates to go to and
    // if the pen should be up or down during this movement
    // NOTE(klek): Depending on our implementation (file download or
    // direct communication) the data buffer should be of different size

    
    int i = 0;

    while (1) {
        // Go 20 steps in positive X direction
        for (i = 0; i < 100; i++) {
            move(POS_X);
        }

        // Go 20 steps in negative X direction
        for (i = 0; i < 100; i++) {
            move(NEG_X);
        }

        // Go 20 steps in positive Y direction
        for (i = 0; i < 100; i++) {
            move(POS_Y);
        }

        // Go 20 steps in negative Y direction
        for (i = 0; i < 100; i++) {
            move(NEG_Y);
        }

        osi_Sleep(100);
    }
    // Wait here
//    while (1);
}

// Task to gather data from the source, dechiper it and finally pack it into the databuffer for
// orders to be carried out by workTask
static void dataTask(void* params)
{
    long retVal = -1;
    // Start the wlan
    retVal = wlanStart();
    // ERROR CHECKING
    
    // Start by connecting to wlan
    retVal = wlanConnect();
    if ( retVal < 0 ) {
        // We are pretty much fucked
        Report("Couldn't connect to network\n\r");
        while(1);
    }

    // We should now be connected to the network
//    Report("Connected to 
}
