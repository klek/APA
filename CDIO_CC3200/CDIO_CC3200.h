/*
#include "uart_if.h"

#define WORK_TASKSTACKSIZE   10000
#define DATA_TASKSTACKSIZE 20000
#define SCALE 	10
#define DATABUFSIZE 10
#define COORDSIZE 6
#define MAXDUTY 1650
#define MINDUTY 800
#define PENDOWN_PWM 1500
#define PENUP_PWM 1000
#define PENMAX_PWM 800
#define MAXSTEPSY 100000 // Set correct value after measurements
#define MAXSTEPSX 100000 // Set correct value after measurements
#define SIZE_OF_ORDER 3  // The size we want the order struct to be...sizeof() says 6...


enum directions {
    POS_X,
    POS_Y,
    NEG_X,
    NEG_Y
};


// Possible values for the pen field in order struct
// Could also be return value for checkPen-function
enum {
    PEN_UP,
    PEN_DOWN,
    END_OF_DATA
};

struct step{
	int x;
	int y;
	int xScale;
	int yScale;
};

struct order{
	char pen;
	unsigned short int x;
	unsigned short int y;
};


void brytarIntY(unsigned int index);
void brytarIntX(unsigned int index);
void InsertPenInt(unsigned int index);
static void move(unsigned char direction);
//void read_file(struct order coord[], UART_Handle uart, unsigned int *addedCommands);
static unsigned int charToInt(unsigned char* temp, unsigned char size);
void moveToOrigin();

*/
