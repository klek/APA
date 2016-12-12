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

#define TASKSTACKSIZE   100000
#define QUAD 	1
#define DIR2	8
#define SCALE 	1
#define COORDSIZE 10000
#define COMMANDSIZE 6
#define MAXDUTY 1650
#define MINDUTY 800
#define MAXSTEPSY 100000 // Set correct value after measurements
#define MAXSTEPSX 100000 // Set correct value after measurements

enum directions {
    POS_X,
    POS_Y,
    NEG_X,
    NEG_Y
};

struct step{
	int x;
	int y;
	int xScale;
	int yScale;
};

struct command{
	short int x;
	short int y;
	char pen;
};

/* GLOBAL VARABLES */
struct step steps;
int originFlagX = 0; // 0 - Not origin. 1 - Origin
int originFlagY = 0; // 0 - Not origin. 1 - Origin

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

void brytarIntY(unsigned int index);
void brytarIntX(unsigned int index);
static void move(unsigned char direction);
void read_file(struct command coord[], UART_Handle uart, unsigned int *addedCommands);
void moveToOrigin();
void charToInt(char temp[], short int *xCoord, short int *yCoord);

void brytarIntY(unsigned int index)
{
	int i;
	for (i = 0; i < 70 ; i++)
	{
		move(POS_Y);
	}
	steps.y = 0;
	originFlagY = 1;
	IntPendClear(INT_GPIOA0);
}

void brytarIntX(unsigned int index)
{
	int i;
	for (i = 0; i < 70 ; i++)
	{
		move(POS_X);
	}
	steps.x = 0;
	originFlagX = 1;
	IntPendClear(INT_GPIOA3);
}


void moveTask(UArg arg0, UArg arg1)
{
	UART_Handle uart;
	UART_Params uartParams;
	PWM_Handle pwm1;
	PWM_Params params;

	// Get port and pins
	unsigned int GPIO9Port = 0; // PWM (Also LED)
	unsigned char GPIO9Pin;		// PWM (Also LED)
	GPIO_IF_GetPortNPin(9, &GPIO9Port, &GPIO9Pin); // PWM (Also LED)

	steps.x = 0;
	steps.y = 0;
	unsigned int addedCommands = 0;
	//char coord[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	struct command coord[COORDSIZE];

	//GPIO_IF_LedOff(MCU_ALL_LED_IND);

	/* Initiate the PWM */
	uint16_t   pwmPeriod = 20000;      // Period and duty in microseconds
	uint16_t   duty = 1000;
	uint16_t   dutyInc = 50;
	PWM_Params_init(&params);
	params.period = pwmPeriod;
	pwm1 = PWM_open(Board_PWM0, &params);
	MAP_UtilsDelay(20000);
	PWM_setDuty(pwm1, duty);

	/* Create a UART with data processing off. */
	UART_Params_init(&uartParams);
	uartParams.writeDataMode = UART_DATA_BINARY;
	uartParams.readDataMode = UART_DATA_BINARY;
	uartParams.readReturnMode = UART_RETURN_FULL;
	uartParams.readEcho = UART_ECHO_ON;
	uartParams.baudRate = 9600;
	uart = UART_open(Board_UART0, &uartParams);

	if (pwm1 == NULL) {
		System_abort("Board_PWM0 did not open");
	}

	if (uart == NULL)
	{
		System_abort("Error opening the UART");
	}

	const char Prompt[] = "\fHeeeello!\r\n";
	UART_write(uart, Prompt, sizeof(Prompt));

	read_file(coord,uart, &addedCommands);

	int nCommands; // Iterator for coord
    while (1)
    {
    	for(nCommands = 0; nCommands < addedCommands; nCommands++) // Step through all commands in coord
    	{
			// Move to start coordinate
			while((steps.x != coord[nCommands].x) || (steps.y != coord[nCommands].y))
			{
				if(steps.x < coord[nCommands].x)
				{
					move(POS_X);
				}
				else if (steps.x > coord[nCommands].x)
				{
					move(NEG_X);
				}

				if ((steps.x != coord[nCommands].x) && (steps.y != coord[nCommands].y)) // Only sleep if diagonal movement
				{
					Task_sleep(20);
					//MAP_UtilsDelay(20000);
				}

				if(steps.y < coord[nCommands].y)
				{
					move(POS_Y);
				}
				else if (steps.y > coord[nCommands].y)
				{
					move(NEG_Y);
				}
			}
    	}
    }
}

void read_file(struct command coord[], UART_Handle uart, unsigned int *addedCommands)
{
	char msg;
	unsigned int nCommands = 0;
	char temp[COMMANDSIZE];
	int i;

	UART_read(uart, &msg, 1);

	while((int) msg != EOF)
	{
		switch (msg)
		{
			case 'U': // Character for pen up

				for(i = 0; i < COMMANDSIZE; i++) // Read input chars into temp array
				{
					UART_read(uart, &msg, 1);
					temp[i] = msg;
				}

				charToInt(temp, &coord[nCommands].x, &coord[nCommands].y); // Convert and scale chars to x and y coordinates in short ints
				coord[nCommands].pen = 'U';
				nCommands++; // Increase number of commands in coord
				break;

			case 'N': // Character for pen down

				for(i = 0; i < COMMANDSIZE; i++) // Read input chars into temp array
				{
					UART_read(uart, &msg, 1);
					temp[i] = msg;
				}
				charToInt(temp, &coord[nCommands].x, &coord[nCommands].y); // Convert and scale chars to x and y coordinates in short ints
				coord[nCommands].pen = 'N';
				nCommands++; // Increase number of commands in coord
				break;

			case 'O':
					moveToOrigin();
				break;

			default:
				break;

		}
		if (nCommands < COORDSIZE)
		{
			UART_read(uart, &msg, 1);
		}
		else break;
	}
	(*addedCommands) = nCommands; // How many commands added into coord
}


void moveToOrigin()
{
	/* Move to the origin */
	while(!originFlagX) // Move until not in the origin
	{
		move(NEG_X);
	}

	while(!originFlagY) // Move until not in the origin
	{
		move(NEG_Y);
	}
}

void charToInt(char temp[], short int *xCoord, short int *yCoord)
{
	int i;
	for(i = 0; i < COMMANDSIZE; i++)
	{
		if ((temp[i] < 58) && (temp[i] > 47))
		{
			temp[i] = temp[i]-48;
		}
		else
		{
			// Error message?
		}
	}

	// Decode the message coordinates to ints
	*xCoord = 100*temp[0]+10*temp[1]+temp[2];
	*yCoord = 100*temp[3]+10*temp[4]+temp[5];

	// Scale millimeters to steps of the motors
	*xCoord = (*xCoord)*SCALE;
	*yCoord = (*yCoord)*SCALE;
}


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
    	if (steps.x < MAXSTEPSX)
    	{
			// Both motors should spin counterclockwise
			GPIO_IF_Set(7, GPIO7Port, GPIO7Pin, 1); // Direction for right driver GP7 - pin 62
			GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 1); // Direction for left driver GP11 - pin 2
			steps.x++;
			originFlagX = 0;
    	}
        break;

    case POS_Y:
    	if (steps.y < MAXSTEPSY)
    	{
			// Left motor should spin clockwise, right motor should spin counterclockwise
			GPIO_IF_Set(7, GPIO7Port, GPIO7Pin, 1); // Direction for right driver GP7 - pin 62
			GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 0); // Direction for left driver GP11 - pin 2
			steps.y++;
			originFlagY = 0;
    	}
        break;

    case NEG_X:
    	if (!originFlagX)
    	{
    		// Both motors should spin clockwise
			GPIO_IF_Set(7, GPIO7Port, GPIO7Pin, 0); // Direction for right driver GP7 - pin 62
			GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 0); // Direction for left driver GP11 - pin 2
			steps.x--;
    	}
    	break;

    case NEG_Y:
    	if (!originFlagY)
    	{
			GPIO_IF_Set(7, GPIO7Port, GPIO7Pin, 0); // Direction for right driver GP7 - pin 62
			GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 1); // Direction for left driver GP11 - pin 2
			steps.y--;
    	}
    	break;

    default:
        break;
    }

    // We should always make a step after direction has been set
    MAP_UtilsDelay(20000);
	GPIO_IF_Set(10, GPIO10Port, GPIO10Pin, 1); // Step on - Pin 1 GP10
	MAP_UtilsDelay(20000);
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

	/* install Button callback */
	GPIO_setCallback(BOARD_INT0, brytarIntX); // GPIO28 - Pin 18
	GPIO_setCallback(BOARD_INT1, brytarIntY); // GPIO22 - Pin 15

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

