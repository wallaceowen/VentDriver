// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <Arduino.h>

#include "constants.h"
#include "ports.h"
#include "vnew.h"
#include "ventilator.h"
#include "commander.h"

// The ventilator
Ventilator *ventilator;
char v_buffer[sizeof(Ventilator)];

// Comms interface to the ventilator
Commander*commander;
char c_buffer[sizeof(Commander)];

void setup()
{  
    Serial.begin(115200);
    while (!Serial)
        ;
    pinMode(STEPPER_STEP, OUTPUT);
    pinMode(STEPPER_DIR, OUTPUT);
    pinMode(STEPPER_ENABLE, OUTPUT);

    // while (true)
        // Serial.println("Test");
    // while (true)
    // {
        // digitalWrite(STEPPER_ENABLE, HIGH);
        // delayMicroseconds(2);
        // digitalWrite(STEPPER_ENABLE, LOW);
        // delayMicroseconds(2);
    // };

    ventilator = new (v_buffer) Ventilator();
    commander = new (c_buffer) Commander(ventilator);

    // Change these to suit your stepper if you want
    // stepper.setMaxSpeed(9000);
    // stepper.setAcceleration(2000);
    // stepper.moveTo(7200);
}

void loop()
{
    // Serial.println("loop()");
    commander->loop();
    ventilator->loop();
}
