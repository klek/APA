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
#include "interrupt.h"
#include "prcm.h"

// Simplelink includes
#include "simplelink.h"
#include "simplelink_func.h"

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
#define INIT_TASK_STACK_SIZE      (512)
#define OOB_TASK_PRIORITY         (1)

#define INTERRUPT_PRIORITY        10

#define X_HOME                    (1 << 0)
#define Y_HOME                    (1 << 1)

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

/*
 * Function prototypes
 */
// Interrupt handler for homing-switches
static void homingHandler(void);

// Initialize the components of the board
static void boardInit(void);

// Function to display banner on UART at startup
static void displayBanner(void);

// Task to initialize
static void initTask(void* params);

// This task is supposed to do all the work
static void workTask(void* params);

/*
 * The main function
 */
void main(void)
{
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

    /*
     * Create interrupts
     */
    // Create the interrupt for HOME_X_AXIS
    retVal = osi_InterruptRegister(gpioGetIntBase(HOME_X_AXIS),homingHandler, INTERRUPT_PRIORITY);
    if ( retVal < 0 ) {
        Report("Failed to register the interrupts");
        while(1);
    }

    // Create the interrupt for HOME_Y_AXIS
    retVal = osi_InterruptRegister(gpioGetIntBase(HOME_Y_AXIS), homingHandler, INTERRUPT_PRIORITY);
    if ( retVal < 0 ) {
        Report("Failed to register the interrupts");
    }
    // Enable both interrupts
    gpioEnableInterrupts();
    
    /*
     *Create tasks
     */
    // Create init task
//    retVal = osi_TaskCreate(initTask, (signed char*)"initTask",
//                            INIT_TASK_STACK_SIZE, NULL, OOB_TASK_PRIORITY, NULL);
    if ( retVal < 0 ) {
        Report("Failed to create initTask");
        while(1);
    }
    
    // Create the main task
    retVal = osi_TaskCreate(workTask, (signed char*)"workTask",
                            WORK_TASK_STACK_SIZE, NULL, OOB_TASK_PRIORITY, NULL);
    if ( retVal < 0 ) {
        Report("Failed to create workTask");
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

// Interrupt handler for homing-switches
static void homingHandler(void)
{
    // Get which pins gave the interrupt
//    unsigned long pinState = GPIOIntStatus();

    while(1);
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
}


// Function to test the newly implemented functions
static void workTask(void* params)
{
    // Task setup
 
    // After we have found the homing position
    

    
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
    }
    // Wait here
//    while (1);
}
