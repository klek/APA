/*
 *  ======== CDIO_CC3200.c ========
 */

// Standard includes
#include <stdio.h>
#include <stdint.h>

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

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SDSPI.h>
// #include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/PWM.h>
// #include <ti/drivers/Watchdog.h>

/* Example/Board Header files */
#include "Board.h"

#define TASKSTACKSIZE   10000
#define QUAD 	1
#define DIR2	8
#define SCALE 	1
#define COORDSIZE 14

enum directions {
    POS_X,
    POS_Y,
    NEG_X,
    NEG_Y
};


Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];


void init_step();
void initInt();
//void brytarInt();
void brytarInt(unsigned int index);
void brytarIntYO(unsigned int index);
//void brytarIntYO();
static void move(unsigned char direction);
void charToInt(char coord[], int *xStart, int *yStart, int *xEnd, int *yEnd, int *zPos);


struct step{
	int x;
	int y;
};

struct step steps;


/* variable to be read by GUI Composer */
int count = 0;




//void brytarInt()
void brytarInt(unsigned int index)
{
	/* Stop the motors in this direction */
	/* Decide what switch was activated */

    /* Clear the GPIO interrupt and toggle an LED */

    //GPIO_toggle(Board_LED0);

    if (count++ == 1) {
        count = 0;
    }
}

//void brytarIntYO()
void brytarIntYO(unsigned int index)
{
	/* Stop the motors in this direction */
	/* Decide what switch was activated */

    /* Clear the GPIO interrupt and toggle an LED */

    //GPIO_toggle(Board_LED0);

    if (count++ == 1) {
        count = 0;
    }
}


void moveTask(UArg arg0, UArg arg1)
{
	UART_Handle uart;
	UART_Params uartParams;
    PWM_Handle pwm1;
    PWM_Params params;

	// Get port and pins
	unsigned int GPIO7Port = 0; // Direction right motor
	unsigned char GPIO7Pin; 	// Direction right motor
	unsigned int GPIO9Port = 0; // PWM (Also LED)
	unsigned char GPIO9Pin;		// PWM (Also LED)
	unsigned int GPIO10Port = 0;// Step
	unsigned char GPIO10Pin;	// Step
	unsigned int GPIO11Port = 0;// Direction left motor
	unsigned char GPIO11Pin;	// Direction left motor
	GPIO_IF_GetPortNPin(7, &GPIO7Port, &GPIO7Pin); // Direction right motor
	GPIO_IF_GetPortNPin(9, &GPIO9Port, &GPIO9Pin); // PWM (Also LED)
	GPIO_IF_GetPortNPin(10, &GPIO10Port, &GPIO10Pin); // Step
	GPIO_IF_GetPortNPin(11, &GPIO11Port, &GPIO11Pin); // Direction right motor

	int xStart, xEnd, yStart, yEnd, zPos = 0;
	steps.x = 0;
	steps.y = 0;
	char msg;
	char coord[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	GPIO_IF_LedOff(MCU_ALL_LED_IND);

	/* Create a UART with data processing off. */
	UART_Params_init(&uartParams);
	uartParams.writeDataMode = UART_DATA_BINARY;
	uartParams.readDataMode = UART_DATA_BINARY;
	uartParams.readReturnMode = UART_RETURN_FULL;
	uartParams.readEcho = UART_ECHO_ON;
	uartParams.baudRate = 9600;
	uart = UART_open(Board_UART0, &uartParams);


	/* Initiate the PWM */
    uint16_t   pwmPeriod = 20000;      // Period and duty in microseconds
    uint16_t   duty = 100;
    uint16_t   dutyInc = 200;
    PWM_Params_init(&params);
    params.period = pwmPeriod;
    pwm1 = PWM_open(Board_PWM0, &params);
    if (pwm1 == NULL) {
    	System_abort("Board_PWM0 did not open");
    }


	if (uart == NULL)
	{
		System_abort("Error opening the UART");
	}

	const char Prompt[] = "\fHeeeello!\r\n";
	UART_write(uart, Prompt, sizeof(Prompt));

	int i = 0;
    while (1)
    {
    	// Fetch data (get coordinate)
    	UART_read(uart, &msg, 1);


    	// Decode data (read parameters from coordinate data)
    	switch(msg)
    	{
    		case 'c':
    			i = 0;
    			while(i <= 13)
    			{
    				UART_read(uart, &msg, 1); // Read message

    				// Read
					coord[i] = msg;

					i++;
    			}


    			charToInt(coord, &xStart, &yStart, &xEnd, &yEnd, &zPos);

    			// Move to start coordinate
    			while((steps.x != xStart) || (steps.y != yStart))
    			{
    				if(steps.x < xStart)
    				{
    					move(POS_X);
    				}
    				else if (steps.x > xStart)
    				{
    					move(NEG_X);
    				}

    				if(steps.y < yStart)
    				{
    					move(POS_Y);
    				}
    				else if (steps.y > yStart)
    				{
    					move(NEG_Y);
    				}
    			}

    			// Move from start to end coordinate
    			while((steps.x != xEnd) || (steps.y != yEnd))
				{
					if(steps.x < xEnd)
					{
						move(POS_X);
					}
					else if (steps.x > xEnd)
					{
						move(NEG_X);
					}

					if(steps.y < yEnd)
					{
						move(POS_Y);
					}
					else if (steps.y > yEnd)
					{
						move(NEG_Y);
					}
				}


    			/*
    			GPIO_IF_Set(9, GPIO9Port, GPIO9Pin, 1); // LED on

    			GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 1); // Direction for left driver GP11 - pin 2
				MAP_UtilsDelay(20000);
				GPIO_IF_Set(10, GPIO10Port, GPIO10Pin, 1); // Step - Pin 1 GP10
    			MAP_UtilsDelay(40000);
    			GPIO_IF_Set(10, GPIO10Port, GPIO10Pin, 0); // Step - Pin 1 GP10
				*/
    			/*
    			while(msg != '\n')
    			{
    				UART_read(uart, &msg, 1);
    				UART_write(uart, &msg, 1);
    				//coord[i] = msg;
    				i++;
    			}
    			*/
    			break;

    		case 'x':
    			GPIO_IF_Set(9, GPIO9Port, GPIO9Pin, 0); // LED off

    			GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 0); // Direction for left driver GP11 - pin 2
    			MAP_UtilsDelay(20000);
    			GPIO_IF_Set(10, GPIO10Port, GPIO10Pin, 1); // Step - Pin 1 GP10
    			MAP_UtilsDelay(40000);
    			GPIO_IF_Set(10, GPIO10Port, GPIO10Pin, 0); // Step - Pin 1 GP10

    			break;

    		case 'z':


    			while (duty < pwmPeriod)
    			{
    				PWM_setDuty(pwm1, duty);
    				//MAP_UtilsDelay(10000000);
    				Task_sleep(500);
    		        duty = (duty + dutyInc);

    		        /*
    		         if (duty == pwmPeriod || (!duty)) {
    		            dutyInc = - dutyInc;
    		        }
    		        */

    			}
    			duty = 0;
    			/*
    			if (duty > 2000)
    			{
    				duty = 1000;
    			}
    			else
    			{
    				duty += 500;
    			}
    			PWM_setDuty(pwm1, duty);
*/
    			break;

    		default:
    			break;
    	}
    }
}


void charToInt(char coord[], int *xStart, int *yStart, int *xEnd, int *yEnd, int *zPos)
{
	int i;
	for(i = 0; i < COORDSIZE; i++)
	{
		if ((coord[i] < 58) && (coord[i] > 47))
		{
			coord[i] = coord[i]-48;
		}
		else
		{
			// Error message?
		}
	}

	// Decode the message coordinates to ints
	*xStart = 100*coord[0]+10*coord[1]+coord[2];
	*yStart = 100*coord[3]+10*coord[4]+coord[5];
	*xEnd = 100*coord[6]+10*coord[7]+coord[8];
	*yEnd = 100*coord[9]+10*coord[10]+coord[11];
	*zPos = 10*coord[12]+coord[13];

	// Scale millimeters to steps of the motors
	*xStart = (*xStart)*SCALE;
	*yStart = (*yStart)*SCALE;
	*xEnd = (*xEnd)*SCALE;
	*yEnd = (*yEnd)*SCALE;
}

/*
void move(char & coord, uint16_t duty, uint16_t pwmPeriod)
{
	// Set duty cycle to pwm (move servo)
	//PWM_setDuty(pwm1, duty);
}
*/


// Function to take 1 step in the specified direction according to
// the XY-plane
static void move(unsigned char direction)
{
	unsigned int GPIO7Port = 0; // Direction right motor
	unsigned char GPIO7Pin; 	// Direction right motor
	unsigned int GPIO10Port = 0;// Step
	unsigned char GPIO10Pin;	// Step
	unsigned int GPIO11Port = 0;// Direction left motor
	unsigned char GPIO11Pin;	// Direction left motor
	GPIO_IF_GetPortNPin(7, &GPIO7Port, &GPIO7Pin); // Direction right motor
	GPIO_IF_GetPortNPin(10, &GPIO10Port, &GPIO10Pin); // Step
	GPIO_IF_GetPortNPin(11, &GPIO11Port, &GPIO11Pin); // Direction right motor

    switch(direction)
    {
    case POS_X:
        // Both motors should spin counterclockwise
        //GPIO_IF_LedOn(MCU_GREEN_LED_GPIO); // Right
    	//GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 1);

    	GPIO_IF_Set(7, GPIO7Port, GPIO7Pin, 1); // Direction for right driver GP7 - pin 62
        GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 1); // Direction for left driver GP11 - pin 2
        steps.x++;
        break;

    case POS_Y:
        // Left motor should spin clockwise, right motor should spin counterclockwise
        //GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
        //GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 1);

        GPIO_IF_Set(7, GPIO7Port, GPIO7Pin, 0); // Direction for right driver GP7 - pin 62
		GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 1); // Direction for left driver GP11 - pin 2
		steps.y++;
        break;

    case NEG_X:
        // Both motors should spin clockwise
        //GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
        //GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 0);

    	GPIO_IF_Set(7, GPIO7Port, GPIO7Pin, 0); // Direction for right driver GP7 - pin 62
		GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 0); // Direction for left driver GP11 - pin 2
		steps.x--;
    	break;

    case NEG_Y:
        //GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
        //GPIO_IF_Set(DIR2, GPIO8Port, GPIO8Pin, 0);

    	GPIO_IF_Set(7, GPIO7Port, GPIO7Pin, 1); // Direction for right driver GP7 - pin 62
		GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 0); // Direction for left driver GP11 - pin 2
		steps.y--;
    	break;

    default:
        break;
    }

    // We should always make a step after direction has been set
    MAP_UtilsDelay(40000);
	GPIO_IF_Set(10, GPIO10Port, GPIO10Pin, 1); // Step on - Pin 1 GP10
	MAP_UtilsDelay(40000);
	GPIO_IF_Set(10, GPIO10Port, GPIO10Pin, 0); // Step off - Pin 1 GP10


    //MAP_UtilsDelay(20000);                     // Can we have this smaller?
    //GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
    //MAP_UtilsDelay(20000);                     // Can we have this smaller?
    //GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);

}


/*
 *  ======== main ========
 */

int main(void)
{
    // Call board init functions
    Board_initGeneral();
    Board_initGPIO();
    // Board_initI2C();
    // Board_initSDSPI();
    // Board_initSPI();
    Board_initUART();
    Board_initPWM();
    // Board_initWatchdog();

    Task_Params taskParams;

    // Construct heartBeat Task  thread
    Task_Params_init(&taskParams);
    //taskParams.arg0 = 1000;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    taskParams.instance->name = "echo";
    Task_construct(&task0Struct, (Task_FuncPtr)moveTask, &taskParams, NULL);

    //initInt();

    // Turn on user LED
    GPIO_write(Board_LED0, Board_LED_ON);

    /* install Button callback */
	//GPIO_setCallback(Board_BUTTON0, brytarInt);

	/* Enable interrupts */
	//GPIO_enableInt(Board_BUTTON0);

	/* install Button callback */
	GPIO_setCallback(BOARD_INT0, brytarIntYO); // GPIO28 - Pin 18
	GPIO_setCallback(BOARD_INT1, brytarInt); // GPIO22 - Pin 15

	/* Enable interrupts */
	GPIO_enableInt(BOARD_INT0);
	GPIO_enableInt(BOARD_INT1);

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    // SysMin will only print to the console when you call flush or exit
    System_flush();

    // Start BIOS
    BIOS_start();

    return (0);
}

