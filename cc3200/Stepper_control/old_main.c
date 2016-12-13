/*******************************************************************
   $File:    main.c
   $Date:    Thu, 13 Oct 2016: 13:15
   $Version: 
   $Author:  klek 
   $Notes:   
********************************************************************/

// Standard includes
#include <stdio.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "hw_apps_rcm.h"
#include "pin.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "gpio.h"
#include "utils.h"

// Common interface includes
#include "gpio_if.h"

// #include "pinmux.h"

#define QUAD    1
#define DIR2    8

/*
 * Global variables needed for CCS
 */
//#if defined(ccs)
//extern void (* const g_pfnVectors[])(void);
//#endif

/*
 * Local function prototypes
 */
//static void PinMuxConfig(void);

/*
 * Tasks
 */


/*
 * Local functions
 */
/*
// Board initialization and config
static void BoardInit(void)
{
    // Set the vector table base
    // if TI-RTOS would be used, the OS itself would do this
#if !defined(USE_TIRTOS)
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#endif

    // Enable Processor
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

// Test function
void SteppingRoutine()
{
    // Initialize the stepcounter for each motor
    int stepLeft = 0;
    int stepRight = 0;

    // Get port and pins
    unsigned int GPIO8Port = 0;
    unsigned char GPIO8Pin;
    GPIO_IF_GetPortNPin(DIR2, &GPIO8Port, &GPIO8Pin);

    //
    // Toggle the lines initially to turn off the LEDs.
    // The values driven are as required by the LEDs on the LP.
    //
    GPIO_IF_LedOff(MCU_ALL_LED_IND);

    while (1)
    {
        while ( (stepLeft >= 0 && stepLeft < 2800) && (stepRight <= 0 && stepRight > -2800) )
        {
            // Increase/Decrease step per motor
            stepLeft++;
            stepRight--;

            // Set directions
            GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
            //GPIO_IF_LedOn(MCU_RED_LED_GPIO);
            GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 1);


            // Step
            MAP_UtilsDelay(20000);
            GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
            MAP_UtilsDelay(20000);
            GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        }

        while ( (stepLeft >= 2800 && stepLeft < 5600) && (stepRight >= -2800 && stepRight < 0) )
        {
            // Increase/Decrease step per motor
            stepLeft++;
            stepRight++;

            // Set directions to clockwise
            GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
//          GPIO_IF_LedOn(MCU_RED_LED_GPIO);
            GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 1);

            // Step
            MAP_UtilsDelay(20000);
            GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
            MAP_UtilsDelay(20000);
            GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        }
#if !QUAD
        tBoolean clockWise = true;
        while ( (stepLeft <= 5600 && stepLeft > 0) )
        {

            if ( clockWise == true ) {
                stepRight++;
                // Set direction of left motor to CW
//              GPIO_IF_LedOff(MCU_RED_LED_GPIO);
                GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 0);
            }
            else {
                stepRight--;
                // Set direction of left motor to CCW
//              GPIO_IF_LedOn(MCU_RED_LED_GPIO);
                GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 1);
            }

            if ( stepRight == 0 ) {
                clockWise = true;
            }
            else if ( stepRight == 2 ) {
                clockWise = false;
            }
            // Increase/Decrease step per motor
            stepLeft--;

            // Set directions of right motor
            GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);

            // Step
            MAP_UtilsDelay(20000);
            GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
            MAP_UtilsDelay(20000);
            GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        }


#else
        while ( (stepLeft <= 5600 && stepLeft > 2800) && (stepRight >= 0 && stepRight < 2800) )
        {
            // Increase/Decrease step per motor
            stepLeft--;
            stepRight++;

            // Set directions
            GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
//          GPIO_IF_LedOff(MCU_RED_LED_GPIO);
            GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 0);

            // Step
            MAP_UtilsDelay(20000);
            GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
            MAP_UtilsDelay(20000);
            GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        }


        while ( (stepLeft <= 2800 && stepLeft > 0) && (stepRight <= 2800 && stepRight > 0) )
        {
            // Increase/Decrease step per motor
            stepLeft--;
            stepRight--;

            // Set directions
            GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
//          GPIO_IF_LedOff(MCU_RED_LED_GPIO);
            GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 0);

            // Step
            MAP_UtilsDelay(20000);
            GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
            MAP_UtilsDelay(20000);
            GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        }
#endif
    }

#if 0
    while(1)
    {
        /*
         * D5 - Green - Pin_02 - DIR
         * D6 - Orange - Pin_01 - STEP
         * D7 - Red - Pin_64 - DIR2
         */
/*
        // Check how many steps has been taken
        if ( stepCount >= 0 && stepCount < 2800 ) {
            // Increment the stepCounter
            stepCount++;
            // Stepping control
            // Set direction low
            GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
            GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        }
        else if ( stepCount >= 2800 && stepCount < 5600 ){
            // Increment the stepcounter
            stepCount++;
            // Set direction high
            GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
            GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        }
        else {
            // Reset the stepcounter
            stepCount = 0;
        }

        // Toggle the step§
        MAP_UtilsDelay(20000);
        GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
        MAP_UtilsDelay(20000);
        GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);

        //
        // Alternately toggle hi-low each of the GPIOs
        // to switch the corresponding LED on/off.
        //
//        MAP_UtilsDelay(8000000);
//        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
//        MAP_UtilsDelay(8000000);
//        GPIO_IF_LedOff(MCU_RED_LED_GPIO);
//        MAP_UtilsDelay(8000000);
//        GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
//        MAP_UtilsDelay(8000000);
//        GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
//        MAP_UtilsDelay(8000000);
//        GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
//        MAP_UtilsDelay(8000000);
//        GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
    }
#endif
}
*/
/*
 * Main function
 */
/*int main()
{
    // Initialize board config
    BoardInit();

    // Power on the corresponding GPIOs
    PinMuxConfig();

    GPIO_IF_LedConfigure(LED1|LED2|LED3);

    GPIO_IF_LedOff(MCU_ALL_LED_IND);

    SteppingRoutine();

    // End program
    return 0;
}
*/
/*
static void PinMuxConfig(void)
{
    //
    // Enable Peripheral Clocks
    //
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_64 for GPIOOutput
    //
//    MAP_PinTypeGPIO(PIN_64, PIN_MODE_0, false);
//    MAP_GPIODirModeSet(GPIOA1_BASE, 0x2, GPIO_DIR_MODE_OUT);

    MAP_PinTypeGPIO(PIN_63, PIN_MODE_0, false);
    // Get port and pins
    unsigned int GPIO8Port = 0;
    unsigned char GPIO8Pin;
    GPIO_IF_GetPortNPin(DIR2, &GPIO8Port, &GPIO8Pin);
    MAP_GPIODirModeSet(GPIO8Port, GPIO8Pin, GPIO_DIR_MODE_OUT);

    //
    // Configure PIN_01 for GPIOOutput
    //
    MAP_PinTypeGPIO(PIN_01, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x4, GPIO_DIR_MODE_OUT);

    //
    // Configure PIN_02 for GPIOOutput
    //
    MAP_PinTypeGPIO(PIN_02, PIN_MODE_0, false);
    MAP_GPIODirModeSet(GPIOA1_BASE, 0x8, GPIO_DIR_MODE_OUT);
}

*/
