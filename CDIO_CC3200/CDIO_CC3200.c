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
#define DATABUFSIZE 10
#define MAXDUTY 1650
#define MINDUTY 800
#define PENDOWN_PWM 1500
#define PENUP_PWM 1000
#define PENMAX_PWM 800
#define MAXSTEPSY 100000 // Set correct value after measurements
#define MAXSTEPSX 100000 // Set correct value after measurements
//#define SIZE_OF_ORDER 3  // The size we want the order struct to be...sizeof() says 6... Use PACKED_ORDER_SIZE instead


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

// Work task
Task_Struct task0Struct;
Char task0Stack[WORK_TASKSTACKSIZE];

// Data task
Task_Struct task1Struct;
Char task1Stack[DATA_TASKSTACKSIZE];

// Array for all data to be unpacked to orders
static unsigned char printData[DATABUFSIZE * PACKED_ORDER_SIZE];
static struct order orderToWorkTask[PACKED_ORDER_SIZE]; // 3 orders large

/* GLOBAL VARABLES */
struct step steps;
int originFlagX = 0; // 0 - Not origin. 1 - Origin
int originFlagY = 0; // 0 - Not origin. 1 - Origin
int insertPen = 1;
bool workRead = false; // Only Data will be allowed to write to this, Work only read this flag
bool dataRead = true; // Only Work will be allowed to write to this, Data only read this flag


//void charToInt(char temp[], short int *xCoord, short int *yCoord);

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

void InsertPenInt(unsigned int index)
{
	insertPen = 0;
	IntPendClear(INT_GPIOA1);
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
    	if (workRead)
    	{
    		dataRead = false;
			for(nCommands = 0; nCommands < PACKED_ORDER_SIZE; nCommands++) // Step through all commands in printData
			{
				orderToWorkTask[nCommands].x *= SCALE;
				orderToWorkTask[nCommands].y *= SCALE;

				if (orderToWorkTask[nCommands].pen != 'E')
				{
					if (orderToWorkTask[nCommands].pen == 'D')
					{
						PWM_setDuty(pwm1, PENDOWN_PWM);
					}
					else if (orderToWorkTask[nCommands].pen == 'U')
					{
						PWM_setDuty(pwm1, PENUP_PWM);
					}

					Task_sleep(100);
					// Move to start coordinate
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

						if ((steps.x != orderToWorkTask[nCommands].x) && (steps.y != orderToWorkTask[nCommands].y)) // Only sleep if diagonal movement
						{
							Task_sleep(20);
							//MAP_UtilsDelay(20000);
						}

						if(steps.y < orderToWorkTask[nCommands].y)
						{
							move(POS_Y);
						}
						else if (steps.y > orderToWorkTask[nCommands].y)
						{
							move(NEG_Y);
						}
					}
				}
				else
				{
					while(1);
				}
			}

			PWM_setDuty(pwm1, PENUP_PWM); // Raise the pen
			dataRead = true;
    	}
    	else
    	{
    		Task_sleep(10);
    	}
    }
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
			if ( ((orderBufferIndex + PACKED_ORDER_SIZE) <= validData) && dataRead) {
				workRead = false;
				orderBufferIndex += unPackData(&printData[orderBufferIndex],&orderToWorkTask[0]);
				orderBufferIndex += unPackData(&printData[orderBufferIndex],&orderToWorkTask[1]);
				orderBufferIndex += unPackData(&printData[orderBufferIndex],&orderToWorkTask[2]);
				workRead = true;
			}
			else {
				// Are we at the end??
				// Or do we simply need to gather more data??
			}
			Task_sleep(1000);
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

