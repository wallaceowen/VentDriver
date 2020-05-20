// ventilator control

#include <Arduino.h>
#include <AccelStepper.h>

#include "ventilator.h"
#include "constants.h"
#include "ports.h"
#include "vnew.h"


// #define DEBUG_SETTERS
// #define DEBUG_VENTILATOR_TIMING
#define DEBUG_VENTILATOR_CHANGES

#define INTERVAL_MSEC 25

static volatile int home_reached = 0;

// Used by placement new, storage for the AccelStepper object constructed
// in the Ventilator's ctor
static char stepper_buffer[sizeof(AccelStepper)];

// So the ISR can get to the ventilator object
static Ventilator *local_ventilator = 0;

// Deal with home button being pressed
void Ventilator::home_triggered()
{
    // If we're homing stop the motor
    if ((m_state == VS_Homing) || (m_state == VS_ExhaleDelay))
    {
        home_reached = 1;

        // Stop the motor
        m_stepper->setCurrentPosition(0);

        // If we were in ExhaleDelay switch us to Inhale
        if (m_state == VS_ExhaleDelay)
            set_vstate(VS_Inhale);
    }
}

void Ventilator::enable()
{
    m_enabled = true;
    m_params_changed = true;
    set_vstate(VS_Inhale);
    digitalWrite(m_enable_pin, STEPPER_ASSERT_STATE);
}

void Ventilator::disable()
{
    m_enabled = false;
    set_vstate(VS_Disable);
    m_params_changed = true;
    digitalWrite(m_enable_pin, STEPPER_DEASSERT_STATE);
}

void Ventilator::home()
{
    m_params_changed = true;
    set_vstate(VS_Home);
    digitalWrite(m_enable_pin, STEPPER_ASSERT_STATE);
}

void Ventilator::if_params_changed_update_vars()
{
    if (m_params_changed)
    {
        m_distance_steps = (m_steps_per_mm*m_length_mm)*m_percent/100;
        int32_t duration_ms_third = ((60L/m_bpm)*MILLISECONDS)/3L;
        m_inhale_duration = duration_ms_third;
        m_exhale_duration = duration_ms_third*2;
        m_inhale_speed = 1.0*m_distance_steps*MILLISECONDS/m_inhale_duration;
        m_exhale_speed = 1.0*m_distance_steps*MILLISECONDS/m_exhale_duration;
        m_params_changed = false;
    }
}

void Ventilator::adjust_speed(float speed)
{
    // Serial.print("adjust_speed(");
    // Serial.print(speed);
    // Serial.println(")");
    if (speed < m_drive_system.max_speed)
        m_stepper->setMaxSpeed(speed);
}

void Ventilator::loop_var_speed()
{
    if_params_changed_update_vars();

    // Serial.println(vstate_strings[m_state]);
    switch(m_state)
    {
        case VS_Disable:
            digitalWrite(m_enable_pin, m_enabled?STEPPER_ASSERT_STATE:STEPPER_DEASSERT_STATE);
            set_vstate(VS_Disabled);
            break;

        case VS_Disabled:
            return;
            break;

        case VS_Exhale:
        {
            adjust_speed(m_exhale_speed);
            m_stepper->moveTo(0);

            set_vstate(VS_ExhaleDelay);
            break;
        }

        case VS_ExhaleDelay:
        {
            long remaining =  m_stepper->distanceToGo();
            if (remaining == 0)
                set_vstate(VS_Inhale);
            break;
        }

        case VS_Inhale:
        {
            adjust_speed(m_inhale_speed);
            m_stepper->moveTo(m_distance_steps);

            set_vstate(VS_InhaleDelay);
            break;
        }

        case VS_InhaleDelay:
        {
            move_t remaining = m_stepper->distanceToGo();

            if (remaining == 0)
                set_vstate(VS_Exhale);
            break;
        }

        case VS_Home:
        {
            home_reached = 0;
            set_vstate(VS_Homing);
            adjust_speed(m_homing_speed);
            // Serial.println("moving to 0");
            move_t past_home = -(m_steps_per_mm*m_length_mm);
            m_stepper->moveTo(past_home);
            set_vstate(VS_Homing);
            break;
        }

        case VS_Homing:
        {
            // This gets set in the home sensor callback
            if (home_reached == 1)
                set_vstate(VS_Homed);

            // This gets set by the return value from accelstepper.run()
            // FIXME should we use this? I think we should keep heading
            // backward until we bump into the home sensor.
            // if (!m_stepper_moving)
                // set_vstate(VS_Homed);
            break;
        }

        case VS_Homed:
        {
            // Nothing to do in homed state.  We will sit here until we
            // receive an Enable message, which will put us in Inhale state
            break;
        }

        default:
        {
            break;
        }
    }
}

/*
 * Setters for BPM and volume
 */
void Ventilator::set_bpm(int bpm)
{
    if (m_bpm != bpm)
    {
        m_bpm = bpm;
        m_params_changed = true;
    }
}
void Ventilator::set_percent(int percent)
{
    if (m_percent != percent)
    {
        m_percent = percent;
        m_params_changed = true;
    }
}

/*
 * Setting acceleration, max_speed, steps/mm, length and homing speed are parameter setting functions,
 * typically called once at startup to configure the venilator code for the system to which it's attached.
 */
void Ventilator::set_accel(float accel)
{
    m_drive_system.max_accel = accel;
    m_stepper->setAcceleration(accel);
    m_params_changed = true;
}

void Ventilator::set_max_speed(float max_speed)
{
    m_drive_system.max_speed = max_speed;
    m_params_changed = true;
}

void Ventilator::set_steps_per_mm(move_t steps_per_mm)
{
    m_steps_per_mm = steps_per_mm;
    m_params_changed = true;
}

void Ventilator::set_length(move_t length)
{
    m_length_mm = length;
    m_params_changed = true;
}

void Ventilator::set_homing_speed(speed_t homing_speed)
{
    m_homing_speed = homing_speed;
}

void home_pin_changed()
{
    if (local_ventilator)
        local_ventilator->home_triggered();
}

Ventilator::Ventilator() :
    // Initial default values for drive system
    m_drive_system({DEFAULT_STEPS_PER_MM, DEFAULT_STEPPER_MAX_ACCEL, DEFAULT_STEPPER_MAX_SPEED }),
    m_enabled(false),
    m_state(VS_Disabled),
    m_bpm(MAX_BPM/2),
    m_percent(MAX_VOLUME/2),
    m_length_mm(DEFAULT_PISTON_LEN_MM),
    m_steps_per_mm(DEFAULT_STEPS_PER_MM),
    m_params_changed(true),
    m_stepper(new (stepper_buffer) AccelStepper(AccelStepper::DRIVER, STEPPER_STEP, STEPPER_DIR)),
    m_enable_pin(STEPPER_ENABLE),
    m_inhale_duration(0),
    m_exhale_duration(0),
    m_inhale_speed(0.0),
    m_exhale_speed(0.0),
    m_homing_speed(HOME_STEPPER_SPEED),
    m_stepper_moving(false)
{
    local_ventilator = this;
    digitalWrite(m_enable_pin, STEPPER_DEASSERT_STATE);
    pinMode(HOME_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(HOME_PIN), home_pin_changed, LOW);

    // Give the stepper driver a nice fat 5uS pulse
    m_stepper->setMinPulseWidth(5U);

    // Start off with max, accel.  Use set_accel to set a lower acceleration
    m_stepper->setAcceleration(m_drive_system.max_accel);

    // We are at 0. TODO: Home us here?  Or is that dangerous and we just need to
    // use the home command when we're ready?
    m_stepper->setCurrentPosition(0L);
}

void Ventilator::loop()
{
    loop_var_speed();
    m_stepper_moving = m_stepper->run();
}
