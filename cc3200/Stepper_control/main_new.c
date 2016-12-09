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
#define TASK_STACK_SIZE      (1024)
#define OOB_TASK_PRIORITY    (1)

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

/*
 * Function prototypes
 */
// Initialize the components of the board
static void boardInit(void);

// Function to display banner on UART at startup
static void displayBanner(void);

// Function to test the stepping pattern
static void steppingTask(void* params);

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
//    retVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
//    if ( retVal < 0 ) {
//        Report("Unable to start simplelink spawn task\n\r");
//        while(1);
//    }

    // Create tasks
    retVal = osi_TaskCreate(steppingTask, (signed char*)"steppingTask",
                            TASK_STACK_SIZE, NULL, OOB_TASK_PRIORITY, NULL);

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

// Function to test the newly implemented functions
static void steppingTask(void* params)
{
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
