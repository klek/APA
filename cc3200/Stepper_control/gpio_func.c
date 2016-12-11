/*******************************************************************
   $File:    gpio_func.c
   $Date:    Thu, 08 Dec 2016: 16:55
   $Version: 
   $Author:  klek 
   $Notes:   
********************************************************************/

#include "gpio_func.h"

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "rom_map.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"
#include "utils.h"

/*
 * Static variables
 */ 
static unsigned long ulReg[] =
{
    GPIOA0_BASE,
    GPIOA1_BASE,
    GPIOA2_BASE,
    GPIOA3_BASE,
    GPIOA4_BASE,
};

static unsigned long ulIntReg[] =
{
    INT_GPIOA0,
    INT_GPIOA1,
    INT_GPIOA2,
    INT_GPIOA3
};

/*
 * Static functions
 */
// Gets the port and pin number for the specified GPIO-pin
static void gpioGetPortNPin(unsigned char pinNr, unsigned int* gpioPort, unsigned char* gpioPin)
{
    *gpioPin = 1 << (pinNr % 8);
    *gpioPort = ulReg[pinNr / 8 ];
}


/*
 * Function declarations
 */

// Pinmux config for the project
// Enables and configures the pins used by the printer
void pinMuxConfig(void)
{
    // Variables to set-up pin-configs
    unsigned int gpioPort = 0;
    unsigned char gpioPin = 0;
    
    // Enable Peripheral clocks
    MAP_PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA1, PRCM_RUN_MODE_CLK);

    /*
     * UART-pins for communication with PC through console
     */
    // PIN_55 for UART0 TX
    MAP_PinTypeUART(PIN_55, PIN_MODE_3);
    // PIN_57 for UART0 RX
    MAP_PinTypeUART(PIN_57, PIN_MODE_3);

    /*
     * Configuring pins for motordrivers
     */
    // R_MOTOR_DIR_PIN setup as output GPIO
    MAP_PinTypeGPIO(PIN_62, PIN_MODE_0, false);
    gpioGetPortNPin(R_MOTOR_DIR_PIN, &gpioPort, &gpioPin);
    MAP_GPIODirModeSet(gpioPort, gpioPin, GPIO_DIR_MODE_OUT);

    // L_MOTOR_DIR_PIN setup as output GPIO
    MAP_PinTypeGPIO(PIN_02, PIN_MODE_0, false);
    gpioGetPortNPin(L_MOTOR_DIR_PIN, &gpioPort, &gpioPin);
    MAP_GPIODirModeSet(gpioPort, gpioPin, GPIO_DIR_MODE_OUT);

    // MOTOR_STEP_PIN setup as output GPIO
    MAP_PinTypeGPIO(PIN_01, PIN_MODE_0, false);
    gpioGetPortNPin(MOTOR_STEP_PIN, &gpioPort, &gpioPin);
    MAP_GPIODirModeSet(gpioPort, gpioPin, GPIO_DIR_MODE_OUT);

    /*
     * Configuring interrupt pins for homing-switches
     */
    // HOME_X_AXIS setup as input GPIO
    MAP_PinTypeGPIO(PIN_15, PIN_MODE_0, false);
    gpioGetPortNPin(HOME_X_AXIS, &gpioPort, &gpioPin);
    MAP_GPIODirModeSet(gpioPort, gpioPin, GPIO_DIR_MODE_IN);
    MAP_GPIOIntTypeSet(gpioPort, gpioPin, GPIO_RISING_EDGE | GPIO_HIGH_LEVEL);

    // HOME_Y_AXIS setup as input GPIO
    MAP_PinTypeGPIO(PIN_18, PIN_MODE_0, false);
    gpioGetPortNPin(HOME_Y_AXIS, &gpioPort, &gpioPin);
    MAP_GPIODirModeSet(gpioPort, gpioPin, GPIO_DIR_MODE_IN);
    MAP_GPIOIntTypeSet(gpioPort, gpioPin, GPIO_RISING_EDGE | GPIO_HIGH_LEVEL);

    // NOTE(klek): Configuring this will remove the possibility to debug the device
    //             cause this is one of the JTAG-pins
    // END_X_AXIS setup as input GPIO
    //MAP_PinTypeGPIO(PIN_17, PIN_MODE_0, false);
    //gpioGetPortNPin(END_X_AXIS, &gpioPort, &gpioPin);
    //MAP_GPIODirModeSet(gpioPort, gpioPin, GPIO_DIR_MODE_IN);

    // NOTE(klek): Configuring this will remove the possibility to debug the device
    //             cause this is one of the JTAG-pins
    // END_Y_AXIS setup as input GPIO
    //MAP_PinTypeGPIO(PIN_16, PIN_MODE_0, false);
    //gpioGetPortNPin(END_Y_AXIS, &gpioPort, &gpioPin);
    //MAP_GPIODirModeSet(gpioPort, gpioPin, GPIO_DIR_MODE_IN);

    /*
     * Configuring PWM pins for control of the servo
     */
    // PWM_SERVO setup as output PWM
    MAP_PinTypeTimer(PIN_64, PIN_MODE_3);
}

// Sets the corresponding pin to high
void gpioSetPin(unsigned char pinNr)
{
    unsigned char gpioPin = 1 << (pinNr % 8);
    unsigned int gpioPort = ulReg[pinNr / 8];

    // Set corresponding value in bitmask. Value is 1
    unsigned int val = 1 << (pinNr % 8);
    
    MAP_GPIOPinWrite(gpioPort, gpioPin, val);
}

// Sets the corresponding pin to low
void gpioUnSetPin(unsigned char pinNr)
{
    unsigned char gpioPin = 1 << (pinNr % 8);
    unsigned int gpioPort = ulReg[pinNr / 8];

    // Set corresponding value in bitmask. Value is 1
    unsigned int val = 0 << (pinNr % 8);

    MAP_GPIOPinWrite(gpioPort, gpioPin, val);
}

// Gets the value on the corresponding pin
unsigned char gpioGetPin(unsigned char pinNr)
{
    long gpioStatus;
    unsigned char gpioPin = 1 << (pinNr % 8);
    unsigned int gpioPort = ulReg[pinNr / 8];

    gpioStatus = MAP_GPIOPinRead(gpioPort, gpioPin);

    return (gpioStatus >> (pinNr % 8));
}

// Pulses the corresponding pin - sets pin high, then low
void gpioPulsePin(unsigned char pinNr, unsigned int delay)
{
    gpioTogglePin(pinNr);
    // TODO(klek): Fix this for a specified amount of time, argument???
    MAP_UtilsDelay(delay);
    gpioTogglePin(pinNr);
}

// Toggles the corresponding pin
void gpioTogglePin(unsigned char pinNr)
{
    if ( gpioGetPin(pinNr) ) {
        gpioUnSetPin(pinNr);
    }
    else {
        gpioSetPin(pinNr);
    }
}

// Setups interrupt
void gpioEnableInterrupts(unsigned char pinNr)
{
    // Variables to set-up pin-configs
    unsigned int gpioPort = 0;
    unsigned char gpioPin = 0;

    // Enable the interrupt
    gpioGetPortNPin(pinNr, &gpioPort, &gpioPin);
    MAP_GPIOIntClear(gpioPort, gpioPin);
    MAP_GPIOIntEnable(gpioPort, gpioPin);
}

// Return the interrupt port for the specified input
int gpioGetIntBase(unsigned char pinNr)
{
    return (ulIntReg[pinNr / 8]);
}

// Clears the interrupt
void gpioClearInt(unsigned char pinNr)
{
    // Variables to set-up pin-configs
    unsigned int gpioPort = 0;
    unsigned char gpioPin = 0;
    
    gpioGetPortNPin(pinNr, &gpioPort, &gpioPin);

    MAP_GPIOIntClear(gpioPort, gpioPin);
}
