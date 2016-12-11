/*******************************************************************
   $File:    gpio_func.h
   $Date:    Thu, 08 Dec 2016: 16:55
   $Version: 
   $Author:  klek 
   $Notes:   
********************************************************************/

#if !defined(GPIO_FUNC_H)
#define GPIO_FUNC_H

/*
 * Macros
 */

// Pin definitions for the steppers
#define L_MOTOR_DIR_PIN  7   // GPIO7 - PIN62
#define R_MOTOR_DIR_PIN  11  // GPIO11 - PIN02
#define MOTOR_STEP_PIN   10  // GPIO10 - PIN01

// Pin definitions for homing switches
#define HOME_X_AXIS      22  // GPIO22 - PIN15 - Brytare 4
#define HOME_Y_AXIS      28  // GPIO1 - PIN18 - Brytare 3
#define END_X_AXIS       24  // GPIO24 - PIN17 - Brytare 2
#define END_Y_AXIS       23  // GPIO23 - PIN16 - Brytare 1

// Pin definitions for PWM module
#define PWM_SERVO        9   // GPIO9 - PIN64


/*
 * Function prototypes
 */

// Pinmux config for the project
// Enables and configures the pins used by the printer
void pinMuxConfig(void);

// Sets the corresponding pin to high
void gpioSetPin(unsigned char pinNr);

// Sets the corresponding pin to low
void gpioUnSetPin(unsigned char pinNr);

// Gets the value on the corresponding pin
unsigned char gpioGetPin(unsigned char pinNr);

// Pulses the corresponding pin - sets pin high, then low
void gpioPulsePin(unsigned char pinNr, unsigned int delay);

// Toggles the corresponding pin
void gpioTogglePin(unsigned char pinNr);

// Setups interrupt
void gpioEnableInterrupts(unsigned char pinNr);

// Return the interrupt port for the specified input
int gpioGetIntBase(unsigned char pinNr);

// Clears the interrupt
void gpioClearInt(unsigned char pinNr);


#endif
