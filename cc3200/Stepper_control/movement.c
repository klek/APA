/*******************************************************************
   $File:    cnc_movement.c
   $Date:    Thu, 08 Dec 2016: 16:26
   $Version: 
   $Author:  klek 
   $Notes:   
********************************************************************/

#include "movement.h"
#include "gpio_func.h"

// Driverlib includes
#include "rom_map.h"
#include "utils.h"

// Function to take 1 step in the specified direction according to
// the XY-plane
void move(unsigned char direction)
{
    // TODO(klek): Change pin names to something more obvious
    // Proposal of pin names
    // R_MOTOR_DIR_PIN
    // L_MOTOR_DIR_PIN
    // MOTOR_STEP_PIN
    
    // TODO(klek): Use same function to set pins. Follow-up on the above todo.
    switch(direction)
    {
    case POS_X:
        // Both motors should spin counterclockwise
        //GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
        //GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 1);
        gpioSetPin(L_MOTOR_DIR_PIN);
        gpioSetPin(R_MOTOR_DIR_PIN);
        break;

    case POS_Y:
        // Left motor should spin clockwise, right motor should spin counterclockwise
        //GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
        //GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 1);
        gpioSetPin(L_MOTOR_DIR_PIN);
        gpioUnSetPin(R_MOTOR_DIR_PIN);
        break;

    case NEG_X:
        // Both motors should spin clockwise
        //GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
        //GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 0);
        gpioUnSetPin(L_MOTOR_DIR_PIN);
        gpioUnSetPin(R_MOTOR_DIR_PIN);
        break;

    case NEG_Y:
        // Left motor should spin counterclockwise, right motor should spin clockwise
        //GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
        //GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 0);
        gpioUnSetPin(L_MOTOR_DIR_PIN);
        gpioSetPin(R_MOTOR_DIR_PIN);
        break;
        
    default:
        break;
    }

    // We should always make a step after direction has been set
    //MAP_UtilsDelay(20000);                     // Can we have this smaller?
    //GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
    //MAP_UtilsDelay(20000);                     // Can we have this smaller?
    //GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);

    // Between a direction change and a step there is a needed a certain setup-time of
    // 650 ns
    MAP_UtilsDelay(20000);
    // Indicates we want to pulse the MOTOR_STEP_PIN with a certain delay
    // This delay cannot be larger than a frequency of 250 kHz
    gpioPulsePin(MOTOR_STEP_PIN, 20000);
}


// Activates the pen (pushes it down onto the paper)
void activatePen(void)
{
    // Should simply call with a predefined value for PWM output to the
    // servo motor
}

// Deactivates the pen (lifts it from the paper)
void deactivatePen(void)
{
    // Should simply call with a predefined value for PWM output to the
    // servo motor to lift the pen
}

// Checks the position of the pen
unsigned char checkPen(void);
