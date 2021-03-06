# VentDriver
## Ventilator piston stepper driver subsystem
This code provides the low-level driving of a stepper motor-based piston or bellows ventilator mechanism, implementing timing for inhale and exhale, speed and acceleration for desired BPM and volume.  It is designed to be programmed into a microprocessor that is connected to a ventilator control system over a moderately high speed serial interface, providing a dedicated low cost micro for controlling the reciprocating movement of the ventilator mechanics.

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

All commands consist of a single char possibly followed by a parameter with no intervening whitespace, terminated by "\r" by default. (TERM is an enumeration constant in the class implementing the command interpreter, which is a simple switch statement on first char followed by the appropriate sscanf() call).

The home sensor is monitored by an edge-sensitive interrupt, which just sets a volatile that is checked in the ventilator 
state machine.

## Notes

### Principal use case
You are designing/prototyping a moving mechansim style ventilator.  You have some interesing control ideas, some thoughts on measuring flow, and just want to have some drop in piece of subsystem that could assume responsibilities for moving your diaphragm/piston/bellows/ambu-bag in and out in the classic breathe-for-me pattern of 1/3 inhale and 2/3 exhale for a specified breaths per minute.

### Requirements
You will need a spare serial interface over which you can send short ASCII text messages at 115.2K, or an SPI interface and a chip select to talk to me on the input end, and a stepper driver with step, direction and enable, so I can tell the stepper motor what to do.

### Driving steppers
stm32 output pins swing from 0 to 3.3v, and the typical TB6600-based stepper driver for driving the larger NEMA23 steppers uses opto couplers with 560 ohm resistors, you will need to level shift the step, direction and enable pins with a transistor, either NPN or N-channel FET that pulls the cathode of the opto coupler's LED to ground, and tie the anode input pin, which passes through that 560 ohm stepper, to the 5V rail so you generate sufficient current to turn on the LED in the opto coupler.  Or, alternatively, replace the 560 ohm resistor on the stepper driver board with something closer to 300 ohms and tie it to the 3v3 rail and drive the opto couplers directly (the LEDs in the opto couplers need about 15mA to get decent current transfer).

### Percentage of volume
This driver is responsible for moving the diaphragm/bellows/piston/bag using a stepper motor in the established timing of one third of a cycle for inhale and two thirds of the cycle for exhale.  It is not aware of the volume moved by the mechanism, only the length of the stroke possible.
This puts the resposnsibility for obtaining the desired volume of air as a percentage of that full stroke on the controlling system, which is also monitoring differential pressure transducers to detect demand, and anything else outside the scope of moving that piston/diaphragm back and forth with a glass-smooth acceleration profile.

### Home sensor
The code monitors a pin for a negative transition, indicating that the home sensor has triggered.  This is not debounced in the code, so I suggest an opto sensor or some signal conditioning.

### Current target
This code targets the STM32F401C 'Black Pill' and the STM32F103C8T6 'Blue Pill'.  It will also run on an ATmega328p as found on Arduino Nano and several others, but at a significant performance penalty.

### Build environment
Builds as-is in Arduino, has one Arduino library dependency: AccelStepper, the optimized stepper motor movement library.
