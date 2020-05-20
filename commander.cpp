
#include "string.h"
#include "commander.h"
#include "vstate.h"

Commander::Commander(Ventilator *ventilator)
    : m_ventilator(ventilator), m_state(CS_Init), m_offset(0)
{
    memset(m_cmd, 0, CMD_BUFFER_SIZE);
    memset(m_reply, 0, CMD_BUFFER_SIZE);
}

// Send a json string telling the state of the ventilator driver
void Commander::report_state()
{
    char report_line[40];

    sprintf(report_line, "{\"state\": \"%s\", ", vstate_strings[m_ventilator->state()]);
    Serial.print(report_line);

    sprintf(report_line, "\"length\": %ld, ", m_ventilator->length());
    Serial.print(report_line);

    sprintf(report_line, "\"steps/mm\": %ld, ", m_ventilator->steps_per_mm());
    Serial.print(report_line);

    sprintf(report_line, "\"accel\": %f, ", m_ventilator->accel());
    Serial.print(report_line);

    sprintf(report_line, "\"max_speed\": %f, ", m_ventilator->max_speed());
    Serial.print(report_line);

    sprintf(report_line, "\"bpm\": %d, ", m_ventilator->bpm());
    Serial.print(report_line);

    sprintf(report_line, "\"percent\": %d", m_ventilator->percent());
    Serial.print(report_line);
    Serial.println("}");
}

/*
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
bool Commander::process_command()
{
    bool result = true;
    int bpm;
    int percent;
    speed_t accel;
    speed_t max_speed;
    speed_t homing_speed;
    move_t length;
    move_t steps_per_mm;

    switch(m_cmd[0])
    {
        case 'B':
            sscanf(m_cmd+1, "%d", &bpm);
            m_ventilator->set_bpm(bpm);
            break;

        case 'P':
            sscanf(m_cmd+1, "%d", &percent);
            m_ventilator->set_percent(percent);
            break;

        case 'A':
            sscanf(m_cmd+1, "%f", &accel);
            m_ventilator->set_accel((float)accel);
            break;

        case 'S':
            sscanf(m_cmd+1, "%f", &max_speed);
            m_ventilator->set_max_speed((float)max_speed);
            break;

        case 'L':
            sscanf(m_cmd+1, "%ld", &length);
            m_ventilator->set_length(length);
            break;

        case 'M':
            sscanf(m_cmd+1, "%ld", &steps_per_mm);
            m_ventilator->set_steps_per_mm(steps_per_mm);
            break;

        case 'E':
            m_ventilator->enable();
            break;

        case 'D':
            m_ventilator->disable();
            break;

        case 'H':
            m_ventilator->home();
            break;

        case 'R':
            report_state();
            break;

        case 'h':
            sscanf(m_cmd+1, "%f", &homing_speed);
            m_ventilator->set_homing_speed(homing_speed);
            break;

        default:
            result = false;
            break;
    }

    return result;
}

void Commander::step_state()
{
    switch(m_state)
    {
        case CS_Init:
            memset(m_cmd, 0, CMD_BUFFER_SIZE);
            memset(m_reply, 0, CMD_BUFFER_SIZE);
            m_offset = 0;
            m_state = CS_Reading;
            break;

        case CS_Reading:
            if (Serial.available())
            {
                int val = Serial.read();
                if (val == TERM)
                {
                    m_cmd[m_offset++] = '\0';
                    m_state = CS_Command;
                }
                else
                {
                    m_cmd[m_offset++] = val;
                }

                if (m_offset > CMD_BUFFER_SIZE)
                {
                    m_offset = 0;
                    strcpy(m_reply, "MSG TOO BIG\n");
                    m_state = CS_SendReply;
                }
            }
            break;

        case CS_Command:
            if (process_command())
                strcpy(m_reply, "OK\n");
            else
                strcpy(m_reply, "ERR\n");
            m_state = CS_SendReply;
            break;

        case CS_SendReply:
            if (strlen(m_reply))
                Serial.println(m_reply);
            m_state = CS_Init;
            break;
    }
}

void Commander::loop()
{
    step_state();
}


