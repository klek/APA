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
#include "simplelink.h"
#include "simplelink_func.h"

// HTTP client libraries
#include <http/client/httpcli.h>
#include <http/client/common.h>

// Free rtos / TI-RTOS includes
#include "osi.h"

// Common includes
#include "uart_if.h"
#include "common.h"

// Special headers
#include "movement.h"
#include "gpio_func.h"
#include "rtos_callbacks.h"

/*
 * Macros
 */
#define WORK_TASK_STACK_SIZE      (2048)
#define DATA_TASK_STACK_SIZE      (4096)
#define INIT_TASK_STACK_SIZE      (512)
#define WORK_TASK_PRIORITY        (3)
#define DATA_TASK_PRIORITY        (1)
#define INIT_TASK_PRIORITY        (2)

#define INTERRUPT_PRIORITY        10

#define ORDER_BUFFER_SIZE         10000 // This would be 100 orders


// Macros for bits in status vector
#define X_HOME                    (1 << 0)
#define Y_HOME                    (1 << 1)
#define HOME_FOUND                (1 << 2)

// Macros for message queue
#define MAX_MSG_LENGTH            (sizeof(struct order) * 2)
#define MAX_Q_LENGTH              5
#define MAX_ORDER_PER_MSG         2     // We allow 2 orders in one message

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
//static struct position myPos;
// Status variable to indicate interrupts
// NOTE(klek): Maybe increase this one to keep track of when to get more data from web?
static unsigned char status;
// The order buffer is declared outside off tasks
// NOTE(klek):
static unsigned char orderBuffer[ORDER_BUFFER_SIZE * PACKED_ORDER_SIZE];

static OsiMsgQ_t workMsg;
static OsiMsgQ_t dataMsg;

/*
 * External stuff
 */

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

// Find home function to move us to origo
static short int goToHome(void);

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
    gpioEnableInterrupt(HOME_X_AXIS);
    gpioEnableInterrupt(HOME_Y_AXIS);
    
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

    // TODO(klek): Disable the interrupt here and reenable it in the task
    // to be able to correct the position and get a safety switch
    gpioDisableInterrupt(HOME_X_AXIS);
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

    // TODO(klek): Disable the interrupt here and reenable it in the task
    // to be able to correct the position and get a safety switch
    gpioDisableInterrupt(HOME_Y_AXIS);
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

    while (1) {
        osi_Sleep(100);
    }
}


// Task to carry out all the orders found in the databuffer
static void workTask(void* params)
{
    // Task setup
    struct position myPos;
    unsigned char currentOrder[MAX_MSG_LENGTH];

    // At start-up we should always find home first
    if ( goToHome() != SUCCESS ) {
        // Something went terrible wrong
        while(1);
    }
    // We should now be at zero x-position without an interrupt set
    myPos.xPosition = 0;
    // We should now be at zero y-position without an interrupt set
    myPos.yPosition = 0;

    // Print out in console
    Report("Home position found\n\r");
    
    // Indicate for further use that we have found home
    status |= HOME_FOUND;

    // Start the forever loop
    while (1) {
        // Wait for work from dataTask
        osi_MsgQRead(&workMsg, currentOrder, OSI_WAIT_FOREVER);

        // When we get a message we should decipher it
        // The message is a struct of type order
        // and there should be 2 messages in each currentOrder
        struct order* nextOrder = (struct order*)&currentOrder;
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
    // Variables used by this task
    long retVal = -1;
    int orderBufferEnd = 0;
    int orderBufferIndex = 0;
    struct sockaddr_in address;
    HTTPCli_Struct cli;

    // Dummy declaration of buffer
    struct order orderToWorkTask[MAX_ORDER_PER_MSG];
    
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

    // Create the HTTP connection
    // and verify that it works
    retVal = setupHTTPConnection(&cli, &address);
    if ( retVal < 0 ) {
        Report("Failed to create and setup the HTTP-connection\n\r");
    }
    
    // Gather data from the web, by using the HTTP connection
    // After this call, the orderBuffer should be full of data.
    // retVal should contain the number of elements
    orderBufferEnd = fetchAndParseData(&cli, orderBuffer, ORDER_BUFFER_SIZE * SIZE_OF_ORDER);

    // Put this data in the message q to workTask
    // Maybe give this task priority to be able to fill the queue
    // and gather more data while workTask is working??

    while (1) {
        // Read out data from the buffer array
        // Sort the data into a struct order
        if ( orderBufferIndex < (ORDER_BUFFER_SIZE * PACKED_ORDER_SIZE) ) {
            if ( (orderBufferIndex + 3) <= orderBufferEnd ) {
                orderBufferIndex += unPackData(&orderBuffer[orderBufferIndex],&orderToWorkTask[0]);
                orderBufferIndex += unPackData(&orderBuffer[orderBufferIndex],&orderToWorkTask[1]);
            }
            else {
                // Are we at the end??
                // Or do we simply need to gather more data??
            }
            // Queue this message to the workTask
            osi_MsgQWrite(&workMsg, (void *)&orderToWorkTask, OSI_WAIT_FOREVER); // OSI_NO_WAIT?? Maybe we then loose info :S
        }
        else {
            // Well this wasn't to good was it??
        }
    }
}

// Find home function to move us to origo
static short int goToHome(void)
{
    // Start by going to X_HOME
    // Wait for status-flag X_HOME to come true in status reg
    while ( !(status & X_HOME) ) {
        move(NEG_X);
    }
    // Adjust position to not trigger interrupt
    while ( (status & X_HOME) ) {
        // Remove the flag
        status &= ~X_HOME;
        // Enable interrupts
        gpioEnableInterrupt(HOME_X_AXIS);
        move(POS_X);
    }    
    // Follow up by going to Y_HOME
    while ( !(status & Y_HOME) ) {
        move(NEG_Y);
    }
    //Adjust position to not trigger interrupt
    while ( (status & Y_HOME) ) {
        // Remove the flag
        status &= ~Y_HOME;
        // Enable interrupts
        gpioEnableInterrupt(HOME_Y_AXIS);
        move(POS_Y);
    }

    return SUCCESS;
}