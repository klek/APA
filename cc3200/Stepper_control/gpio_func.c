/*******************************************************************
   $File:    gpio_func.c
   $Date:    Thu, 08 Dec 2016: 16:55
   $Version: 
   $Author:  klek 
   $Notes:   
********************************************************************/

#include "gpio_func.h"

// Driverlib includes
#include "hw_memmap.h"
#include "rom_map.h"

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


/*
 * Function declarations
 */

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
void gpioGetPin(unsigned char pinNr)
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
