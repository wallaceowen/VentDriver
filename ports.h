// ports.h

#ifndef __ports_h
#define __ports_h

#define USING_STM32

#ifdef USING_STM32
#define STEPPER_ENABLE   PA4
#define STEPPER_STEP     PA5
#define STEPPER_DIR      PA6
#define HOME_PIN         PA7
#else
#define STEPPER_STEP     5
#define STEPPER_DIR     6
#define STEPPER_ENABLE     7
#define HOME_PIN      8
#endif

#endif
