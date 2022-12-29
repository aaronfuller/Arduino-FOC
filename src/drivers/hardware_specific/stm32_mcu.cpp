
#include "../hardware_api.h"
#include "stm32_mcu.h"

//#define SIMPLEFOC_STM32_DEBUG

#ifdef SIMPLEFOC_STM32_DEBUG
void printTimerCombination(int numPins, PinMap* timers[], int score);
int getTimerNumber(int timerIndex);
#endif

#ifndef SIMPLEFOC_STM32_MAX_PINTIMERSUSED
#define SIMPLEFOC_STM32_MAX_PINTIMERSUSED 12
#endif
int numTimerPinsUsed;
PinMap* timerPinsUsed[SIMPLEFOC_STM32_MAX_PINTIMERSUSED];

// default pwm parameters
#define _PWM_RESOLUTION 12 // 12bit
#define _PWM_RANGE 4095.0 // 2^12 -1 = 4095
#define _PWM_FREQUENCY 25000 // 25khz
#define _PWM_FREQUENCY_MAX 50000 // 50khz

typedef struct STM32DriverParams {
  HardwareTimer* timers[6] = {NULL};
  uint32_t channels[6];
  long pwm_frequency;
  float dead_zone;
  uint8_t interface_type;
} STM32DriverParams;

// setting pwm to hardware pin - instead analogWrite()
void _setPwm(HardwareTimer *HT, uint32_t channel, uint32_t value, int resolution)
{
  // TODO - remove commented code
  // PinName pin = digitalPinToPinName(ulPin);
  // TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(pin, PinMap_PWM);
  // uint32_t index = get_timer_index(Instance);
  // HardwareTimer *HT = (HardwareTimer *)(HardwareTimer_Handle[index]->__this);
  
  HT->setCaptureCompare(channel, value, (TimerCompareFormat_t)resolution);
}

// function setting the high pwm frequency to the supplied pins
// - BLDC motor - 3PWM setting
// - hardware speciffic
void* _configure3PWM(long pwm_frequency,const int pinA, const int pinB, const int pinC) {
  if (numTimerPinsUsed+3 > SIMPLEFOC_STM32_MAX_PINTIMERSUSED) {
    SIMPLEFOC_DEBUG("STM32-DRV: ERR: too many pins used");
    return (STM32DriverParams*)SIMPLEFOC_DRIVER_INIT_FAILED;
  }
  if( !pwm_frequency || !_isset(pwm_frequency) ) pwm_frequency = _PWM_FREQUENCY; // default frequency 25khz
  else pwm_frequency = _constrain(pwm_frequency, 0, _PWM_FREQUENCY_MAX); // constrain to 50kHz max
  // center-aligned frequency is uses two periods
  pwm_frequency *=2;

  int pins[3] = { pinA, pinB, pinC };
  PinMap* pinTimers[3] = { NP, NP, NP };
  if (findBestTimerCombination(3, pins, pinTimers)<0)
    return (STM32DriverParams*)SIMPLEFOC_DRIVER_INIT_FAILED;

  HardwareTimer* HT1 = _initPinPWM(pwm_frequency, pinTimers[0]);
  HardwareTimer* HT2 = _initPinPWM(pwm_frequency, pinTimers[1]);
  HardwareTimer* HT3 = _initPinPWM(pwm_frequency, pinTimers[2]);
  
  uint32_t channel1 = STM_PIN_CHANNEL(pinTimers[0]->function);
  uint32_t channel2 = STM_PIN_CHANNEL(pinTimers[1]->function);
  uint32_t channel3 = STM_PIN_CHANNEL(pinTimers[2]->function);

  STM32DriverParams* params = new STM32DriverParams {
    .timers = { HT1, HT2, HT3 },
    .channels = { channel1, channel2, channel3 },
    .pwm_frequency = pwm_frequency
  };
  timerPinsUsed[numTimerPinsUsed++] = pinTimers[0];
  timerPinsUsed[numTimerPinsUsed++] = pinTimers[1];
  timerPinsUsed[numTimerPinsUsed++] = pinTimers[2];

  _alignTimersNew();

  return params;
}

// function setting the pwm duty cycle to the hardware
// - BLDC motor - 3PWM setting
//- hardware speciffic
void _writeDutyCycle3PWM(float dc_a,  float dc_b, float dc_c, void* params){
  // transform duty cycle from [0,1] to [0,4095]
  _setPwm(((STM32DriverParams*)params)->timers[0], ((STM32DriverParams*)params)->channels[0], _PWM_RANGE*dc_a, _PWM_RESOLUTION);
  _setPwm(((STM32DriverParams*)params)->timers[1], ((STM32DriverParams*)params)->channels[1], _PWM_RANGE*dc_b, _PWM_RESOLUTION);
  _setPwm(((STM32DriverParams*)params)->timers[2], ((STM32DriverParams*)params)->channels[2], _PWM_RANGE*dc_c, _PWM_RESOLUTION);
}

#ifdef SIMPLEFOC_STM32_DEBUG

int getTimerNumber(int timerIndex) {
  #if defined(TIM1_BASE)
    if (timerIndex==TIMER1_INDEX) return 1;
  #endif
  #if defined(TIM2_BASE)
    if (timerIndex==TIMER2_INDEX) return 2;
  #endif
  #if defined(TIM3_BASE)
    if (timerIndex==TIMER3_INDEX) return 3;
  #endif
  #if defined(TIM4_BASE)
    if (timerIndex==TIMER4_INDEX) return 4;
  #endif
  #if defined(TIM5_BASE)
    if (timerIndex==TIMER5_INDEX) return 5;
  #endif
  #if defined(TIM6_BASE)
    if (timerIndex==TIMER6_INDEX) return 6;
  #endif
  #if defined(TIM7_BASE)
    if (timerIndex==TIMER7_INDEX) return 7;
  #endif
  #if defined(TIM8_BASE)
    if (timerIndex==TIMER8_INDEX) return 8;
  #endif
  #if defined(TIM9_BASE)
    if (timerIndex==TIMER9_INDEX) return 9;
  #endif
  #if defined(TIM10_BASE)
    if (timerIndex==TIMER10_INDEX) return 10;
  #endif
  #if defined(TIM11_BASE)
    if (timerIndex==TIMER11_INDEX) return 11;
  #endif
  #if defined(TIM12_BASE)
    if (timerIndex==TIMER12_INDEX) return 12;
  #endif
  #if defined(TIM13_BASE)
    if (timerIndex==TIMER13_INDEX) return 13;
  #endif
  #if defined(TIM14_BASE)
    if (timerIndex==TIMER14_INDEX) return 14;
  #endif
  #if defined(TIM15_BASE)
    if (timerIndex==TIMER15_INDEX) return 15;
  #endif
  #if defined(TIM16_BASE)
    if (timerIndex==TIMER16_INDEX) return 16;
  #endif
  #if defined(TIM17_BASE)
    if (timerIndex==TIMER17_INDEX) return 17;
  #endif
  #if defined(TIM18_BASE)
    if (timerIndex==TIMER18_INDEX) return 18;
  #endif
  #if defined(TIM19_BASE)
    if (timerIndex==TIMER19_INDEX) return 19;
  #endif
  #if defined(TIM20_BASE)
    if (timerIndex==TIMER20_INDEX) return 20;
  #endif
  #if defined(TIM21_BASE)
    if (timerIndex==TIMER21_INDEX) return 21;
  #endif
  #if defined(TIM22_BASE)
    if (timerIndex==TIMER22_INDEX) return 22;
  #endif
  return -1;
}

void printTimerCombination(int numPins, PinMap* timers[], int score) {
  for (int i=0; i<numPins; i++) {
    if (timers[i] == NP)
      SimpleFOCDebug::print("NP");
    else {
      SimpleFOCDebug::print("TIM");
      SimpleFOCDebug::print(getTimerNumber(get_timer_index((TIM_TypeDef*)timers[i]->peripheral)));
      SimpleFOCDebug::print("-CH");
      SimpleFOCDebug::print(STM_PIN_CHANNEL(timers[i]->function));
      if (STM_PIN_INVERTED(timers[i]->function))
        SimpleFOCDebug::print("N");
    }
    SimpleFOCDebug::print(" ");
  }
  SimpleFOCDebug::println("score: ", score);
}

#endif
