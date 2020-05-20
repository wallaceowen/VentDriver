// ventilator control

#ifndef __ventilator_control_h
#define __ventilator_control_h

#include <AccelStepper.h>

#include "constants.h"
#include "vstate.h"
#include "types.h"
#include "drive_system.h"

class Ventilator
{
public:
    Ventilator();

    // Call as often as possible.
    void loop();

    inline VentilatorState state() const { return m_state; }

    // These are getters and setters for the parameters driving the ventilator loop
    int bpm() const { return m_bpm; }
    void set_bpm(int breaths_per_minute);

    int percent() const { return m_percent; }
    void set_percent(int volume_percent);

    float accel() const { return m_drive_system.max_accel; }
    void set_accel(float accel);

    float max_speed() const { return m_drive_system.max_speed; }
    void set_max_speed(float max_speed);

    move_t steps_per_mm() const { return m_steps_per_mm; }
    void set_steps_per_mm(move_t steps_per_mm);

    move_t length() const { return m_length_mm; }
    void set_length(move_t length);

    speed_t homing_speed() const { return m_homing_speed; }
    void set_homing_speed(speed_t homing_speed);

    // Enable, disable, home the ventilator
    void enable();
    void disable();
    void home();

    // inline uint32_t exhale_delay() const { return m_exhale_duration; }
    // inline uint32_t inhale_delay() const { return m_inhale_duration; }

private:
    // friend void vent_home_cb(Ventilator *);
    friend void home_pin_changed();
    friend void vent_enable_cb(const int, const int, void *);

    void adjust_speed(float speed);
    void home_triggered();
    void loop_var_speed();
    void set_vstate(VentilatorState state) { m_state = state; }
    void button_callback(Ventilator *v, const int pin, const int state);
    void adjust_steps_and_speed(int inhale_volume_speed, int exhale_volume_speed);
    void if_params_changed_update_vars();

    DriveSystem     m_drive_system;
    bool            m_enabled;
    VentilatorState m_state;
    int8_t          m_bpm;
    int8_t          m_percent;
    move_t          m_length_mm;
    move_t          m_steps_per_mm;
    bool            m_params_changed;

    AccelStepper   *m_stepper;
    int             m_enable_pin;

    // These are maintained by the state machine
    int32_t m_inhale_duration;
    int32_t m_exhale_duration;
    speed_t m_inhale_speed;
    speed_t m_exhale_speed;
    speed_t m_homing_speed;
    move_t  m_distance_steps;

    bool m_stepper_moving;
};

#endif
