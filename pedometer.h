#ifndef __PEDOMETER_H_
#define __PEDOMETER_H_


#include "mpu6050.h"

#define MAX_AXIS                3
#define MAX_WINDOW              3
#define SAMPLING_MODEL_NUMBER   60
#define MODEL_STANDARD_VALUE    20  



void pedometer_init(void);
uint32_t pedometer_stepCalc(void);
uint16_t stepCount;

#endif //__PEDOMETER_H_
