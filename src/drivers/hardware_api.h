#ifndef HARDWARE_UTILS_DRIVER_H
#define HARDWARE_UTILS_DRIVER_H

#include "../common/foc_utils.h"
#include "../common/time_utils.h"
#include "../communication/SimpleFOCDebug.h"


// flag returned if driver init fails
#define SIMPLEFOC_DRIVER_INIT_FAILED ((void*)-1)

// generic implementation of the hardware specific structure
// containing all the necessary driver parameters
// will be returned as a void pointer from the _configurexPWM functions
// will be provided to the _writeDutyCyclexPWM() as a void pointer
typedef struct GenericDriverParams {
  int pins[6];
  long pwm_frequency;
  float dead_zone;
} GenericDriverParams;

/** 
 * Configuring PWM frequency, resolution and alignment
 * - BLDC driver - 3PWM setting
 * - hardware specific
 * 
 * @param pwm_frequency - frequency in hertz - if applicable
 * @param pinA pinA bldc driver
 * @param pinB pinB bldc driver
 * @param pinC pinC bldc driver
 * 
 * @return -1 if failed, or pointer to internal driver parameters struct if successful
 */
void* _configure3PWM(long pwm_frequency, const int pinA, const int pinB, const int pinC);

/** 
 * Function setting the duty cycle to the pwm pin (ex. analogWrite())
 * - BLDC driver - 3PWM setting
 * - hardware specific
 * 
 * @param dc_a  duty cycle phase A [0, 1]
 * @param dc_b  duty cycle phase B [0, 1]
 * @param dc_c  duty cycle phase C [0, 1]
 * @param params  the driver parameters
 */ 
void _writeDutyCycle3PWM(float dc_a,  float dc_b, float dc_c, void* params);

#endif