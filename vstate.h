// ventilator state

#ifndef __vstate_h
#define __vstate_h

enum VentilatorState { VS_Disable, VS_Disabled,
    VS_Inhale, VS_InhaleDelay,
    VS_Exhale, VS_ExhaleDelay,
    VS_Full,
    VS_Home, VS_Homing, VS_Homed };
extern const char *vstate_strings[];

#endif
