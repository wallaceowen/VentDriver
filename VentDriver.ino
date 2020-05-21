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

// When home pin goes low inform the ventilator
void home_pin_changed()
{
        ventilator->home_triggered();
}

void setup()
{  
    Serial.begin(115200);
    while (!Serial)
        ;

    pinMode(STEPPER_STEP, OUTPUT);
    pinMode(STEPPER_DIR, OUTPUT);
    pinMode(STEPPER_ENABLE, OUTPUT);

    ventilator = new (v_buffer) Ventilator();
    attachInterrupt(digitalPinToInterrupt(HOME_PIN), home_pin_changed, LOW);
    commander = new (c_buffer) Commander(ventilator);
}

void loop()
{
    commander->loop();
    ventilator->loop();
}
