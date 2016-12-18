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
#include "uart_if.h"
#include "wlan.h"
#include <ti/mw/wifi/cc3x00/simplelink/include/simplelink.h>
//#include "simplelink.h"

// Common interface includes
#include "gpio_if.h"

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SDSPI.h>
// #include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/WiFi.h>
// #include <ti/drivers/Watchdog.h>

/* Example/Board Header files */
#include "Board.h"
/* Include Network Header files */
#include "simplelink_func.h"
#include "network_cred.h"

#define WORK_TASKSTACKSIZE   10000
#define DATA_TASKSTACKSIZE 20000
#define SCALE 	10
#define DATABUFSIZE 20000
#define MAXDUTY 1650
#define MINDUTY 800
#define PENDOWN_PWM 1550 // 1500 tidigare // 1350 efter det
#define PENUP_PWM 1000 //1000
#define PENMAX_PWM 800
#define MAXSTEPSY 2000
#define MAXSTEPSX 2850
#define MINSTEPSY 0
#define MINSTEPSX 0
#define MAXORDERS_INROW 3
#define SCALE_PWM 0.175		// k in y = kx+m for duty cycle of pwm
//#define DELTA_PWM 50		// The +- delta from PENDOWN_PWM
//#define SIZE_OF_ORDER 3  	// The size we want the order struct to be...sizeof() says 6... Use PACKED_ORDER_SIZE instead


enum directions {
    POS_X,
    POS_Y,
    NEG_X,
    NEG_Y
};

struct step{
	int x;
	int y;
};



void brytarIntY(unsigned int index);
void brytarIntX(unsigned int index);
void InsertPenInt(unsigned int index);
static void move(unsigned char direction);
//void read_file(struct order coord[], UART_Handle uart, unsigned int *addedCommands);
static unsigned int charToInt(unsigned char* temp, unsigned char size);
void moveToOrigin();
int pwmDutyDown(int y);

// Work task
Task_Struct task0Struct;
Char task0Stack[WORK_TASKSTACKSIZE];

// Data task
Task_Struct task1Struct;
Char task1Stack[DATA_TASKSTACKSIZE];

// Semaphores
Semaphore_Struct semStruct_ReadWrite;
Semaphore_Handle semHandle_ReadWrite;

// Array for all data to be unpacked to orders
static unsigned char printData[DATABUFSIZE * PACKED_ORDER_SIZE];
static struct order orderToWorkTask[PACKED_ORDER_SIZE]; // 3 orders large

/* GLOBAL VARABLES */
struct step steps;
int originFlagX = 0; // 0 - Not origin. 1 - Origin
int originFlagY = 0; // 0 - Not origin. 1 - Origin
int insertPen = 1;
bool firstTime = true;
bool extremePositions = false; // Used for not moving outside boundaries


//void charToInt(char temp[], short int *xCoord, short int *yCoord);

void brytarIntY(unsigned int index)
{
	int i;
	for (i = 0; i < 70 ; i++)
	{
		move(POS_Y);
	}
	steps.y = 1;
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
	steps.x = 1;
	originFlagX = 1;
	IntPendClear(INT_GPIOA3);
}

void InsertPenInt(unsigned int index)
{
	insertPen = 0;
	IntPendClear(INT_GPIOA1);
}

void moveToOrigin()
{
	/* Move to the origin */
	while(!originFlagX) // Move until not in the origin
	{
		steps.x = 1;
		move(NEG_X);
	}

	while(!originFlagY) // Move until not in the origin
	{
		steps.y = 1;
		move(NEG_Y);
	}
}

/*
void drawCircle()
{
	for (theta = 0; theta < 360; theta+=20)
	{
		x = x0 + r * cos(theta)
		y = y0 + r * sin(theta)
	}
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
    	if (steps.x < MAXSTEPSX)
    	{
			// Both motors should spin counterclockwise
			GPIO_IF_Set(7, GPIO7Port, GPIO7Pin, 1); // Direction for right driver GP7 - pin 62
			GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 1); // Direction for left driver GP11 - pin 2
			steps.x++;
			originFlagX = 0;
			extremePositions = false;
    	}
    	else
		{
			// Error
			extremePositions = true;
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
			extremePositions = false;
    	}
    	else
    	{
    		// Error
    		extremePositions = true;
    	}
        break;

    case NEG_X:
    	if (!originFlagX && (steps.x > MINSTEPSX))
    	{
    		// Both motors should spin clockwise
			GPIO_IF_Set(7, GPIO7Port, GPIO7Pin, 0); // Direction for right driver GP7 - pin 62
			GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 0); // Direction for left driver GP11 - pin 2
			steps.x--;
			extremePositions = false;
    	}
    	else
    	{
    		// Error
			extremePositions = true;
    	}
    	break;

    case NEG_Y:
    	if (!originFlagY && (steps.y > MINSTEPSY))
    	{
			GPIO_IF_Set(7, GPIO7Port, GPIO7Pin, 0); // Direction for right driver GP7 - pin 62
			GPIO_IF_Set(11, GPIO11Port, GPIO11Pin, 1); // Direction for left driver GP11 - pin 2
			steps.y--;
			extremePositions = false;
    	}
    	else
		{
			// Error
			extremePositions = true;
		}
    	break;

    default:
        break;
    }

    if (!extremePositions)
    {
		// We should always make a step after direction has been set
		MAP_UtilsDelay(20000);
		GPIO_IF_Set(10, GPIO10Port, GPIO10Pin, 1); // Step on - Pin 1 GP10
		MAP_UtilsDelay(20000);
		GPIO_IF_Set(10, GPIO10Port, GPIO10Pin, 0); // Step off - Pin 1 GP10
    }

    //MAP_UtilsDelay(20000);                     // Can we have this smaller?
    //GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
    //MAP_UtilsDelay(20000);                     // Can we have this smaller?
    //GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);

}


int pwmDutyDown(int y)
{
	int duty = -SCALE_PWM*y + PENDOWN_PWM;
	return duty;
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

	steps.x = 2850;
	steps.y = 2000;
	int xDelta = 0;
	int yDelta = 0;
	//unsigned int addedCommands = 0;
	//char coord[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	//GPIO_IF_LedOff(MCU_ALL_LED_IND);

	/* Initiate the PWM */
	uint16_t   pwmPeriod = 20000;      // Period and duty in microseconds
	uint16_t   duty = 1000;
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

	//const char Prompt[] = "\fHeeeello!\r\n";
	//UART_write(uart, Prompt, sizeof(Prompt));

	PWM_setDuty(pwm1, PENUP_PWM); // Set the lowest height for servo motor to insert pen

	moveToOrigin();

	PWM_setDuty(pwm1, PENDOWN_PWM); // Set the lowest height for servo motor to insert pen

	while(insertPen)
	{
		Task_sleep(50);
	}
	PWM_setDuty(pwm1, PENUP_PWM); // Set the lowest height for servo motor to insert pen

	//read_file(printData, uart, &addedCommands);

	int nCommands; // Iterator for printData
    while (1)
    {
		if (!firstTime)
		{
			 // Get access to resource //
			Semaphore_pend(semHandle_ReadWrite, BIOS_WAIT_FOREVER);

			for(nCommands = 0; nCommands < PACKED_ORDER_SIZE; nCommands++) // Step through all commands in printData
			{
				// Scale to correct number of steps
				orderToWorkTask[nCommands].x *= SCALE;
				orderToWorkTask[nCommands].y *= SCALE;

				// Calculate delta
				xDelta = orderToWorkTask[nCommands].x - steps.x;
				yDelta = orderToWorkTask[nCommands].y - steps.y;

				// Make positive if negative
				xDelta = (xDelta*xDelta)/xDelta;
				yDelta = (yDelta*yDelta)/yDelta;

				//if ((orderToWorkTask[nCommands].pen == 'D' && (xDelta > MAXORDERS_INROW) || (yDelta > MAXORDERS_INROW))); // Do nothing
				if((orderToWorkTask[nCommands].y > MINSTEPSY) && (orderToWorkTask[nCommands].y < MAXSTEPSY) &&
						(orderToWorkTask[nCommands].x > MINSTEPSX) && (orderToWorkTask[nCommands].x < MAXSTEPSX))
				{
					if (orderToWorkTask[nCommands].pen == 'E')
					{
						PWM_setDuty(pwm1, PENUP_PWM);
						MAP_UtilsDelay(80000);
						moveToOrigin();
						while(1);
					}
					else if (orderToWorkTask[nCommands].pen == 'D')
					{
						//PWM_setDuty(pwm1, PENDOWN_PWM);
						PWM_setDuty(pwm1, pwmDutyDown(orderToWorkTask[nCommands].y));
					}
					else if (orderToWorkTask[nCommands].pen == 'U')
					{
						PWM_setDuty(pwm1, PENUP_PWM);
					}

					//Task_sleep(100); // Give some time for the pen to move up or down
					MAP_UtilsDelay(200000);

					// Move to coordinate
					while((steps.x != orderToWorkTask[nCommands].x) || (steps.y != orderToWorkTask[nCommands].y))
					{
						if(steps.x < orderToWorkTask[nCommands].x)
						{
							move(POS_X);
						}
						else if (steps.x > orderToWorkTask[nCommands].x)
						{
							move(NEG_X);
						}

						//if ((steps.x != orderToWorkTask[nCommands].x) && (steps.y != orderToWorkTask[nCommands].y) &&
						//		(orderToWorkTask[nCommands].pen != 'U'))
						if ((steps.x != orderToWorkTask[nCommands].x) && (steps.y != orderToWorkTask[nCommands].y))
						{
							//Task_sleep(20); // If drawing and going diagonal, needed for the stepper motors
							MAP_UtilsDelay(100000);
						}

						if(steps.y < orderToWorkTask[nCommands].y)
						{
							move(POS_Y);
						}
						else if (steps.y > orderToWorkTask[nCommands].y)
						{
							move(NEG_Y);
						}

						/*
						// Move in y-direction aswell, althought when pen is up prevent diagonal movement
						if ((orderToWorkTask[nCommands].pen == 'D') ||
								((orderToWorkTask[nCommands].pen == 'U') && (steps.x == orderToWorkTask[nCommands].x)))
						{
							if(steps.y < orderToWorkTask[nCommands].y)
							{
								move(POS_Y);
							}
							else if (steps.y > orderToWorkTask[nCommands].y)
							{
								move(NEG_Y);
							}
						}
						*/
					}
				}
				else
				{
					//const char Prompt[] = "\fX\r\n";
					//UART_write(uart, Prompt, sizeof(Prompt));
				}
			}
		}
		else Task_sleep(10);

		// Release access to protected area //
		Semaphore_post(semHandle_ReadWrite);
		Task_sleep(100);
    }
}

// Task to gather data from the source, dechiper it and finally pack it into the databuffer for
// orders to be carried out by workTask
void dataTask(UArg arg0, UArg arg1)
{
	while(insertPen)
	{
		Task_sleep(100);
	}

    WiFi_Params params;
    WiFi_Handle handle;
    // Variables used by this task
    long retVal = -1;
    int validData = 0;
    int orderBufferIndex = 0;

    struct sockaddr_in address;
    HTTPCli_Struct cli;

    // Dummy declaration of buffer
    //unsigned char orderBuffer[ORDER_BUFFER_SIZE * sizeof(struct order)];
    //Report("Size of the order struct: %i \n\r", sizeof(struct order));


    /* Open WiFi */
    WiFi_Params_init(&params);
    params.bitRate = 5000000; /* Set bit rate to 5 MHz */

    handle = WiFi_open(Board_WIFI, Board_WIFI_SPI, NULL, &params);
    if (handle == NULL) {
    System_abort("Error opening WiFi\n");
    }

    // Start the wlan
    retVal = wlanStart();
    // ERROR CHECKING

    // Start by connecting to wlan
    retVal = wlanConnect();
    if ( retVal < 0 ) {
        // We are pretty much fucked
       // Report("Couldn't connect to network\n\r");
        while(1);
    }


    // We should now be connected to the network

    // Create the HTTP connection
    // and verify that it works
    retVal = setupHTTPConnection(&cli, &address);
    if ( retVal < 0 ) {
       // Report("Failed to create and setup the HTTP-connection\n\r");
    }

    // Gather data from the web, by using the HTTP connection
    // After this call, the orderBuffer should be full of data.
    // retVal should contain the number of elements
    validData = fetchAndParseData(&cli, printData, DATABUFSIZE * PACKED_ORDER_SIZE);


    // Put this data in the message q to workTask
    // Maybe give this task priority to be able to fill the queue
    // and gather more data while workTask is working??

    while (1) // Make struct array
    {
    	if ( orderBufferIndex < (DATABUFSIZE * PACKED_ORDER_SIZE) ) {
			if ( (orderBufferIndex + PACKED_ORDER_SIZE) <= validData) {
				// Get access to resource //
				Semaphore_pend(semHandle_ReadWrite, BIOS_WAIT_FOREVER);

				orderBufferIndex += unPackData(&printData[orderBufferIndex],&orderToWorkTask[0]);
				orderBufferIndex += unPackData(&printData[orderBufferIndex],&orderToWorkTask[1]);
				orderBufferIndex += unPackData(&printData[orderBufferIndex],&orderToWorkTask[2]);

				// Release access to protected area //
				firstTime = false;
				Semaphore_post(semHandle_ReadWrite);
			}
			else {
				// Are we at the end??
				// Or do we simply need to gather more data??
			}
			Task_sleep(100);
		}
    }

    // Add fetchandParseData here
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
    //Board_initSPI();
    Board_initUART();
    Board_initPWM();
    Board_initWiFi();
    // Board_initWatchdog();

    Task_Params taskParamsWork;
    Task_Params taskParamsData;
    Semaphore_Params semParams_ReadWrite;

    // Construct Movement Task  thread
    Task_Params_init(&taskParamsWork);
    //taskParams.arg0 = 1000;
    taskParamsWork.stackSize = WORK_TASKSTACKSIZE;
    taskParamsWork.stack = &task0Stack;
    taskParamsWork.priority = 2;
    taskParamsWork.instance->name = "WORK";
    Task_construct(&task0Struct, (Task_FuncPtr)moveTask, &taskParamsWork, NULL);

    // Construct Task Data thread
    Task_Params_init(&taskParamsData);
    //taskParams.arg0 = 1000;
    taskParamsData.stackSize = DATA_TASKSTACKSIZE;
    taskParamsData.stack = &task1Stack;
    taskParamsData.priority = 1;
    taskParamsData.instance->name = "DATA";
    Task_construct(&task1Struct, (Task_FuncPtr)dataTask, &taskParamsData, NULL);

    /* Construct a Semaphore object to be use as a resource lock, inital count 1 */
    Semaphore_Params_init(&semParams_ReadWrite);
    Semaphore_construct(&semStruct_ReadWrite, 1, &semParams_ReadWrite);

    /* Obtain instance handle */
    semHandle_ReadWrite = Semaphore_handle(&semStruct_ReadWrite);

	/* install Button callback */
	GPIO_setCallback(BOARD_INT0, brytarIntX); // GPIO28 - Pin 18
	GPIO_setCallback(BOARD_INT1, brytarIntY); // GPIO22 - Pin 15
	GPIO_setCallback(Board_BUTTON1, InsertPenInt); // GPIO13

	/* Enable interrupts */
	GPIO_enableInt(BOARD_INT0);
	GPIO_enableInt(BOARD_INT1);
	GPIO_enableInt(Board_BUTTON1);

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    // SysMin will only print to the console when you call flush or exit
    System_flush();

    // Start BIOS
    BIOS_start();

    return (0);
}

