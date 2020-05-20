# VentDriver
Ventilator piston stepper driver subsystem
Provides the low-level driving of a piston or bellows ventilator using a stepper motor, implementing timing for inhale and exhale

## Commands:
-    B -> set Breaths Per Minute. Parameter: integer 5-36
-    P -> set Percentage of full volume. Parameter: integer 0-100
-    A -> set stepper acceleration in steps/sec/sec float 0.0-infinity (constraints of your stepper and drive current)
-    S -> set stepper max speed in steps/sec float 0.0-infinity (constraints of your stepper and drive current)
-    L -> set length of piston stroke in mm. Parameter: integer 0-32768 (constraints yada yada)
-    M -> set steps per mm of piston stroke. Parameter: integer 0-32768 (constraints yada yada). Include microstepping.
-    E -> Enable the ventilator.  Moves state from disabled or homed to Inhale, runs at rate defined by parameters
-    D -> Disable the ventilator.  Moves state to Disabled, stops stepper.  Doesn't home.
-    H -> Home the stepper - move toward home at HOMING_SPEED until home sensor triggers
-    R -> Send a text in json form reporting the current state and parameters
-    h -> set homing speed in steps/sec

All commands consist of a single char possibly followed by a parameter with no intervening whitespace, terminated by TERM, an enumeration constant in the class implementing the command interpreter (simple switch statement on first char followed by the appropriate sscanf() call).

The home sensor is monitored by an edge-sensitive interrupt, which just sets a volatile that is checked in the ventilator 
state machine.
