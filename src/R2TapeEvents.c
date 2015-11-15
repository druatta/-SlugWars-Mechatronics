/* 
 * File:   R2_TapeEvents.c
 * Author: jpewing
 *
 * Created on November 14, 2015, 9:18 PM
 */

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "AD.h"
#include "R2TapeEvents.h"

/************************** #DEFINES **********************
 * - 4 Tape sensors, so we need 4 ports for our tape sensors
 *  - Using PortV5 for Sensor A
 *  - Using PortV6 for Sensor B
 *  - Using PortV7 for Sensor C
 *  - Using PortV8 for Ball Launcher Sensor
 *
 */

#define DELAY(x)         for (wait = 0; wait <= x; wait++) {asm("nop");}
unsigned int wait = 0;


//// Tape Sensors
#define TOP_TAPE_PIN       AD_PORTV8 // Should be PortV5
#define LEFT_TAPE_PIN      AD_PORTV6
#define RIGHT_TAPE_PIN     AD_PORTV7
#define LAUNCHER_TAPE_PIN  AD_PORTV8


#define TEST_TAPE_SENSOR      TOP_TAPE_PIN

// Analog to Digital Conversion
#define BLACK_HYSTERESIS 900 // DIGITAL_ONE
#define WHITE_HYSTERESIS 800 // DIGITAL_ZERO

// Hysteresis Cap
#define ROOM_LIGHT_LEVEL 1023

//#define TAPE_DEBUG_VERBOSE
#ifdef TAPE_DEBUG_VERBOSE
#include "serial.h"
#include <stdio.h>
#define dbprintf(...) while(!IsTransmitEmpty()); printf(__VA_ARGS__)
#else
#define dbprintf(...)
#endif

typedef enum {
    BLACK_TO_WHITE, WHITE_TO_BLACK
} tapestate_t;

/* @Function CheckTopTape
 * @Param void
 * @Return uint8_t so that the ES_Framework can call it
 * @Author Daniel Ruatta, 11.12/2015
 * @Brief
 * 1. Gets a Tape Reading for the Top Tape Sensor
 * 2. Checks if this reading has passed its next
 * Hysteresis Threshold
 * 3. If so, post an event to the TapeSensorHSM
 * 4. Return TRUE if an event has been posted, FALSE otherwise
 */
ES_Event CheckTapeReading() {
    dbprintf("Entered %s\n", __FUNCTION__);
     /***************** Declarations ****************/
   #define MAX_SENSORS (1<<4)

    // We assume the bot starts in a white area, hence BLACK_TO_WHITE
    static tapestate_t PrevTapeState = BLACK_TO_WHITE;

    // TapeReading, to be given a value with ReadTape()
    uint16_t TapeReading = 0;
    //which sensor is being read
    uint8_t TapeSensor = 0;

    //for the sensor loop
    uint8_t sensor;

     // thisEvent, which will be posted to the HSM on a TAPE_FOUND or
    //TAPE_LOST event
     ES_Event thisEvent;

     thisEvent.EventType = ES_NO_EVENT;

     // The New Tape State, which should be initialized to the current PrevTapeState
    // That way, its value will update at the beginning of every Check()
    tapestate_t NewTapeState = PrevTapeState; // Temp value


    for(sensor=1; sensor != MAX_SENSORS; sensor = sensor<<1){
        switch(sensor){
            case TOP_TAPE_SENSOR:
                dbprintf("Reading Top Sensor\n");
                TapeReading = AD_ReadADPin(TOP_TAPE_PIN);
                TapeSensor = TOP_TAPE_SENSOR;
                break;
            case LEFT_TAPE_SENSOR:
                dbprintf("Reading Left Sensor\n");
                TapeReading = AD_ReadADPin(LEFT_TAPE_PIN);
                TapeSensor = LEFT_TAPE_SENSOR;
                break;
            case RIGHT_TAPE_SENSOR:
                dbprintf("Reading Right Sensor\n");
                TapeReading = AD_ReadADPin(RIGHT_TAPE_PIN);
                TapeSensor = RIGHT_TAPE_SENSOR;
                break;
            case LAUNCHER_TAPE_SENSOR:
                TapeReading = AD_ReadADPin(LAUNCHER_TAPE_PIN);
                TapeSensor = LAUNCHER_TAPE_SENSOR;
                break;
            default: break;
            }

        // Case: Tape Found
        if ((PrevTapeState == BLACK_TO_WHITE) &&
                (TapeReading > BLACK_HYSTERESIS) &&
                (TapeReading < ROOM_LIGHT_LEVEL)) {
            thisEvent.EventType = TAPE_FOUND;
            thisEvent.EventParam |= TapeSensor;
            Post_R2_BJT2_HSM(thisEvent);
            NewTapeState = WHITE_TO_BLACK;
        }

        // Case: Tape Lost
        if ((PrevTapeState == WHITE_TO_BLACK) &&
                (TapeReading < WHITE_HYSTERESIS)) {
            thisEvent.EventType = TAPE_LOST;
            thisEvent.EventParam &= ~TapeSensor;
            Post_R2_BJT2_HSM(thisEvent);
            NewTapeState = BLACK_TO_WHITE;
        }
    }

    // PrevTapeState updated with NewTapeState
    PrevTapeState = NewTapeState;

    dbprintf("Exiting %s\n", __FUNCTION__);
    // return Event to event checker
    return thisEvent;
}

/* @Function CheckLeftTape
 * @Param void
 * @Return uint8_t so that the ES_Framework can call it
 * @Author Daniel Ruatta, 11.12/2015
 * @Brief
 * 1. Gets a Tape Reading for the Left Tape Sensor
 * 2. Checks if this reading has passed its next
 * Hysteresis Threshold
 * 3. If so, post an event to the TapeSensorHSM
 * 4. Return TRUE if an event has been posted, FALSE otherwise
 */
//uint8_t CheckLeftTape(void) {
//    /***************** Declarations ****************/
//    // We assume the bot starts in a white area, hence BLACK_TO_WHITE
//    static tapestate_t PrevTapeState = BLACK_TO_WHITE;
//
//    // The New Tape State, which should be initialized to the current PrevTapeState
//    // That way, its value will update at the beginning of every Check()
//    tapestate_t NewTapeState = PrevTapeState; // Temp value
//
//    // TapeReading, to be given a value with ReadTape()
//    uint16_t TapeReading;
//    // thisEvent, which will be posted to the HSM on a TAPE_FOUND or
//    //TAPE_LOST event
//    ES_Event thisEvent;
//    // returnVal, which will be used by the ES_Framework to see if this event
//    // posted an event. We assume no event initially happens, hence FALSE
//    uint8_t returnVal = FALSE;
//
//    // Read Left Tape Sensor
//    TapeReading = AD_ReadADPin(LEFT_TAPE_PIN);
//
//    // Case: Tape Found
//    if ((PrevTapeState == BLACK_TO_WHITE) &&
//            (TapeReading > BLACK_HYSTERESIS) &&
//            (TapeReading < ROOM_LIGHT_LEVEL)) {
//        thisEvent.EventType = TAPE_FOUND;
//        thisEvent.EventParam = LEFT_TAPE_SENSOR;
//        Post_R2_BJT2_HSM(thisEvent);
//        NewTapeState = WHITE_TO_BLACK;
//        returnVal = TRUE;
//
//    }
//
//    // Case: Tape Lost
//    if ((PrevTapeState == WHITE_TO_BLACK) &&
//            (TapeReading < WHITE_HYSTERESIS)) {
//        thisEvent.EventType = TAPE_LOST;
//        thisEvent.EventParam = LEFT_TAPE_SENSOR;
//        Post_R2_BJT2_HSM(thisEvent);
//        NewTapeState = BLACK_TO_WHITE;
//        returnVal = TRUE;
//    }
//
//    // PrevTapeState updated with NewTapeState
//    PrevTapeState = NewTapeState;
//    // Return TRUE if an event has been detected and posted
//    // FALSE otherwise
//    return returnVal;
//}
//
///* @Function CheckRightTape
// * @Param void
// * @Return uint8_t so that the ES_Framework can call it
// * @Author Daniel Ruatta, 11.12/2015
// * @Brief
// * 1. Gets a Tape Reading for the Left Tape Sensor
// * 2. Checks if this reading has passed its next
// * Hysteresis Threshold
// * 3. If so, post an event to the TapeSensorHSM
// * 4. Return TRUE if an event has been posted, FALSE otherwise
// */
//uint8_t CheckRightTape(void) {
//    /***************** Declarations ****************/
//    // We assume the bot starts in a white area, hence BLACK_TO_WHITE
//    static tapestate_t PrevTapeState = BLACK_TO_WHITE;
//
//    // The New Tape State, which should be initialized to the current PrevTapeState
//    // That way, its value will update at the beginning of every Check()
//    tapestate_t NewTapeState = PrevTapeState; // Temp value
//
//    // TapeReading, to be given a value with ReadTape()
//    uint16_t TapeReading;
//    // thisEvent, which will be posted to the HSM on a TAPE_FOUND or
//    //TAPE_LOST event
//    ES_Event thisEvent;
//    // returnVal, which will be used by the ES_Framework to see if this event
//    // posted an event. We assume no event initially happens, hence FALSE
//    uint8_t returnVal = FALSE;
//
//    // Read Left Tape Sensor
//    TapeReading = AD_ReadADPin(RIGHT_TAPE_PIN);
//
//    // Case: Tape Found
//    if ((PrevTapeState == BLACK_TO_WHITE) &&
//            (TapeReading > BLACK_HYSTERESIS) &&
//            (TapeReading < ROOM_LIGHT_LEVEL)) {
//        thisEvent.EventType = TAPE_FOUND;
//        thisEvent.EventParam = RIGHT_TAPE_SENSOR;
//        Post_R2_BJT2_HSM(thisEvent);
//        NewTapeState = WHITE_TO_BLACK;
//        returnVal = TRUE;
//
//    }
//
//    // Case: Tape Lost
//    if ((PrevTapeState == WHITE_TO_BLACK) &&
//            (TapeReading < WHITE_HYSTERESIS)) {
//        thisEvent.EventType = TAPE_LOST;
//        thisEvent.EventParam = RIGHT_TAPE_SENSOR;
//        Post_R2_BJT2_HSM(thisEvent);
//        NewTapeState = BLACK_TO_WHITE;
//        returnVal = TRUE;
//    }
//
//    // PrevTapeState updated with NewTapeState
//    PrevTapeState = NewTapeState;
//    // Return TRUE if an event has been detected and posted
//    // FALSE otherwise
//    return returnVal;
//}
//
///**************************************************************************
// * @Function CheckLauncherTape
// * @Param void
// * @Return uint8_t so that the ES_Framework can call it
// * @Author Daniel Ruatta, 11.12/2015
// * @Brief
// * 1. Gets a Tape Reading for the Launcher Tape Sensor
// * 2. Checks if this reading has passed its next
// * Hysteresis Threshold
// * 3. If so, post an event to the TapeSensorHSM
// * 4. Return TRUE if an event has been posted, FALSE otherwise
// */
//uint8_t CheckLauncherTape(void) {
//    /***************** Declarations ****************/
//    // We assume the bot starts in a white area, hence BLACK_TO_WHITE
//    static tapestate_t PrevTapeState = BLACK_TO_WHITE;
//
//    // The New Tape State, which should be initialized to the current PrevTapeState
//    // That way, its value will update at the beginning of every Check()
//    tapestate_t NewTapeState = PrevTapeState; // Temp value
//
//    // TapeReading, to be given a value with ReadTape()
//    uint16_t TapeReading;
//    // thisEvent, which will be posted to the HSM on a TAPE_FOUND or
//    //TAPE_LOST event
//    ES_Event thisEvent;
//    // returnVal, which will be used by the ES_Framework to see if this event
//    // posted an event. We assume no event initially happens, hence FALSE
//    uint8_t returnVal = FALSE;
//
//    // Read Left Tape Sensor
//    TapeReading = AD_ReadADPin(LAUNCHER_TAPE_PIN);
//
//    // Case: Tape Found
//    if ((PrevTapeState == BLACK_TO_WHITE) &&
//            (TapeReading > BLACK_HYSTERESIS) &&
//            (TapeReading < ROOM_LIGHT_LEVEL)) {
//        thisEvent.EventType = TAPE_FOUND;
//        thisEvent.EventParam = LAUNCHER_TAPE_SENSOR;
//        Post_R2_BJT2_HSM(thisEvent);
//        NewTapeState = WHITE_TO_BLACK;
//        returnVal = TRUE;
//
//    }
//
//    // Case: Tape Lost
//    if ((PrevTapeState == WHITE_TO_BLACK) &&
//            (TapeReading < WHITE_HYSTERESIS)) {
//        thisEvent.EventType = TAPE_LOST;
//        thisEvent.EventParam = LAUNCHER_TAPE_SENSOR;
//        Post_R2_BJT2_HSM(thisEvent);
//        NewTapeState = BLACK_TO_WHITE;
//        returnVal = TRUE;
//    }
//
//    // PrevTapeState updated with NewTapeState
//    PrevTapeState = NewTapeState;
//    // Return TRUE if an event has been detected and posted
//    // FALSE otherwise
//    return returnVal;
//}
//
///*************************************************************
// * @Function InitTape()
// * @Param void
// * @Return uint8_t
// * @Author Daniel Ruatta, 11.09.2015
// * @Brief
// * - Calls AD_AddPins on the 4 Tape Sensor pins.
// * - Should return SUCCESS, assuming the IO_Ports were all set to INPUTS.
// * - Will return ERROR if any of the IO_Ports were not set to INPUTS.
// * @Usage
// * - To be called with the rest of our hardware initializations
// */

uint8_t InitTape(void) {
    return AD_AddPins(TOP_TAPE_PIN |
            LEFT_TAPE_PIN |
            RIGHT_TAPE_PIN |
            LAUNCHER_TAPE_PIN);
}
