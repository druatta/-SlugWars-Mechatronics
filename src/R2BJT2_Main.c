#include "xc.h"
#include <stdio.h>
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "serial.h"
#include "R2Events.h"
//#include "BOARD.h"



void main(void)
{
    ES_Return_t ErrorType;

    // Hardware initialization function calls
    BOARD_Init();
    SERIAL_Init();
    initMotors();
    InitTape();
    InitBumpers();
    InitBeacon();
    //InitTrackWire();
    // When doing testing, it is usefull to annouce just which program
    // is running.
    
    printf("Starting R2-BJT2...\n");
    printf("using the 2nd Generation Events & Services Framework\n");


//     now initialize the Events and Services Framework and start it running
    ErrorType = ES_Initialize();
    printf("Error type: %d\n",ErrorType);
    if (ErrorType == Success) {

        ErrorType = ES_Run();

    }
    //if we got to here, there was an error
    switch (ErrorType) {
    case FailedPointer:
        printf("Failed on NULL pointer");
        break;
    case FailedInit:
        printf("Failed Initialization");
        break;
    default:
        printf("Other Failure: %d",ErrorType);
        break;
    }
    for (;;)
        ;
}

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
