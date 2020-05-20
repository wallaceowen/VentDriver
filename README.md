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

All commands consist of a single char possibly followed by a parameter with no intervening whitespace, terminated by "\r" by default.

(TERM is an enumeration constant in the class implementing the command interpreter, which is a simple switch statement on first char followed by the appropriate sscanf() call).

The home sensor is monitored by an edge-sensitive interrupt, which just sets a volatile that is checked in the ventilator 
state machine.

## Notes
stm32 output pins swing from 0 to 3.3v, and the typical TB6600-based stepper driver for driving the larger NEMA23 steppers uses opto couplers with 560 ohm resistors, you will need to level shift the step, direction and enable pins with a transistor, either NPN or N-channel FET that pulls the cathode of the opto coupler's LED to ground, and tie the anode input pin, which passes through that 560 ohm stepper, to the 5V rail so you generate sufficient current to turn on the LED in the opto coupler.  Or, alternatively, replace the 560 ohm resistor on the stepper driver board with something closer to 300 ohms and tie it to the 3v3 rail.
