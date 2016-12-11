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
 * Structs
 */
// The struct for movement
struct position {
    // NOTE(klek): What do we need?
    // Should simply contain my position in x and y
    // could also contain the value for steps??

    unsigned int xPosition;
    unsigned int yPosition;
};

// Struct for orders
struct order {
    //struct position goTo;

    // Possibly use this instead of the struct
    unsigned int xCoord;
    unsigned int yCoord;

    unsigned char pen;
};

// Possible direction for the move-function
enum directions {
    POS_X,
    POS_Y,
    NEG_X,
    NEG_Y
};

// Return values for the checkPen function
enum {
    PEN_DOWN,
    PEN_UP
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
