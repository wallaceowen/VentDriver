#ifndef __commander_h
#define __commander_h

#include "ventilator.h"

#define CMD_BUFFER_SIZE 32

/*
 * The Commander class accepts message from the serial port
 *
 * All commands consist of a single char possibly followed by a parameter with no intervening whitespace.
 *
 * Commands:
 *  B -> set Breaths Per Minute. Parameter: integer 5-36
 *  P -> set Percentage of full volume. Parameter: integer 0-100
 *  A -> set stepper acceleration in steps/sec/sec float 0.0-infinity (constraints of your stepper and drive current)
 *  S -> set stepper max speed in steps/sec float 0.0-infinity (constraints of your stepper and drive current)
 *  L -> set length of piston stroke in mm. Parameter: integer 0-32768 (constraints yada yada)
 *  M -> set steps per mm of piston stroke. Parameter: integer 0-32768 (constraints yada yada). Include microstepping.
 *  E -> Enable the ventilator.  Moves state from disabled or homed to Inhale, runs at rate defined by parameters
 *  D -> Disable the ventilator.  Moves state to Disabled, stops stepper.  Doesn't home.
 *  H -> Home the stepper - move toward home at HOMING_SPEED until home sensor triggers
 *  R -> Send a text in json form reporting the current state and parameters
 *  h -> set homing speed in steps/sec
 */
class Commander
{
public:
    enum Commands { C_Init, C_SetBPM, C_SetPercent };
    enum CmdState { CS_Init, CS_Reading, CS_Command, CS_SendReply };
    enum Constants { TERM='\r' };

    Commander(Ventilator *ventilator);
    void loop();

private:
    void step_state();
    bool process_command();
    void report_state();

    Ventilator *m_ventilator;
    CmdState m_state;
    unsigned m_offset;
    char m_cmd[CMD_BUFFER_SIZE];
    char m_reply[CMD_BUFFER_SIZE];
};

#endif

