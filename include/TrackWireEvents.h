/* 
 * File:   TrackWireFrameworkEvents.h
 * Author: Daniel Ruatta
 *
 * Created on 11/11/2015
 *
 * The Track Wire Sensor will be read using this library. \
 * It will use Hysteresis thresholds and a previous Track Wire
 * state to determine if the Track Wire has been lost (TRACK_WIRE_LOST) or found
 * (TRACK_WIRE_FOUND). If so, it will post these events to the framework.
 *
 * Note that this event checker, like all others,
 * requires Init code. This is located at the bottom of
 * the .c file, and should be called in main().
 */

#ifndef TrackWireEVENTS_H
#define	TrackWireEVENTS_H

/**************************************************************************
 * @Function CheckTrackWire
 * @Param void
 * @Return uint8_t so that the ES_Framework can call it
 * @Author Daniel Ruatta, 11.13.2015
 * @Brief
 * 1. Gets the Track Wire's analog value
 * 2. Checks if this reading has passed either of its
 * Hysteresis Thresholds
 * 3. If so, post an event to the TrackWireHSM
 * 4. Return TRUE if an event has been posted, FALSE otherwise
 */
uint8_t CheckTrackWire(void);

/*************************************************************
 * @Function InitTrackWire()
 * @Param void
 * @Return uint8_t
 * @Author Daniel Ruatta, 11.13.2015
 * @Brief
 * - Calls AD_AddPins on the Track Wire pin V04
 * - Should return SUCCESS, assuming the IO_Ports were all set to INPUTS.
 * - Will return ERROR if any of the IO_Ports were not set to INPUTS.
 * @Usage
 * - To be called with the rest of our hardware initializations
 */

uint8_t InitTrackWire();

#endif	/* TrackWireEVENTS_H */

