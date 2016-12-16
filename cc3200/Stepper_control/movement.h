/*******************************************************************
   $File:    movement.h
   $Date:    Thu, 08 Dec 2016: 16:31
   $Version: 
   $Author:  klek 
   $Notes:   
********************************************************************/

#if !defined(MOVEMENT_H)
#define MOVEMENT_H

/*
 * Macros
 */
#define COORD_SIZE         6 // 3 digits for x-coord + 3 digits for y-coord
#define SIZE_OF_ORDER      5 // The size we want the order struct to be...sizeof() says 6...

/*
 * Structs
 */
// The struct for movement
struct position {
    // NOTE(klek): What do we need?
    // Should simply contain my position in x and y
    // could also contain the value for steps??

    unsigned short int xPosition;
    unsigned short int yPosition;
};

// Struct for orders
// Goal is to have only 5 bytes of space occupied
// This will mainly be used for typecasting in arrays
// NOTE(klek): Since our resolution is mm, the largest number of a
// A4-paper is 210x300
// To represent this we need 9 bits for the largest coordinate
// This gives 18 bits total for 2 coordinates. which would need 3 bytes leaving
// a rest of 6 bits unused...
struct order {
    //struct position goTo;

    unsigned char pen;
    // Possibly use this instead of the struct
    unsigned short int xCoord;
    unsigned short int yCoord;
};

// Possible direction for the move-function
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


/*
 * Function prototypes
 */ 

// Moves the printer head 1 step in the specified direction
void move(unsigned char direction);

// Activates the pen (pushes it down onto the paper)
void activatePen(void);

// Deactivates the pen (lifts it from the paper)
void deactivatePen(void);

// Checks the position of the pen
unsigned char checkPen(void);

#endif
