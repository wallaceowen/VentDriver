// drive system header

#ifndef __drive_system_h
#define __drive_system_h

// Describe the drive system
struct DriveSystem
{
    unsigned steps_per_mm; /* gearing, microstepping, etc */
    float    max_accel; /* steps/sec/sec */
    float    max_speed; /* steps/sec */
};

#endif
