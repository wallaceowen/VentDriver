// constants

#ifndef __constants_h
#define __constants_h

#include <Arduino.h>

// equivalent to rpm of motor
#define MIN_BPM  5
#define MAX_BPM 36

// Convenience constant
#define MILLISECONDS 1000L

enum Dir {Forward = LOW, Reverse = HIGH };

#define ENA_STATE HIGH
#define DIS_STATE LOW

// Constrain volume so we don't overdrive the motor
#define MIN_VOLUME   5
#define MAX_VOLUME 100
#define HALF_VOLUME ((MIN_VOLUME+MAX_VOLUME)/2)
#define MAX_VOLUME_THRESHOLD 93

#define AD_BITS 10
#define AD_MAX ((1<<AD_BITS)-1)

#define DEFAULT_STEPS_PER_MM 10
#define DEFAULT_PISTON_LEN_MM 800L

#define DEFAULT_STEPPER_MAX_ACCEL 5000L
#define DEFAULT_STEPPER_MAX_SPEED 48000L
#define HOME_STEPPER_SPEED 400

#define HOMING_SPEED 127

#define SECS_IN_MIN 60L
#define MSEC_PER_SEC 1000L

#define STEPPER_ASSERT_STATE LOW
#define STEPPER_DEASSERT_STATE HIGH

#define MIN_BPM_MSEC (SECS_IN_MIN*MSEC_PER_SEC/MIN_BPM)
#define MAX_BPM_MSEC (SECS_IN_MIN*MSEC_PER_SEC/MAX_BPM)

#endif
