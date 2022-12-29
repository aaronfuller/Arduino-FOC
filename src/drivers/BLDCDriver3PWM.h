#ifndef BLDCDriver3PWM_h
#define BLDCDriver3PWM_h

#include "../common/base_classes/BLDCDriver.h"
#include "../common/foc_utils.h"
#include "../common/time_utils.h"
#include "../common/defaults.h"

#include "stm32g4xx_hal_tim.h"
#include "stm32g4xx_hal_gpio.h"

#define ARR_VALUE 2047.0f
#define DRIVER_ENABLE_ACTIVE_HIGH false

#if (DRIVER_ENABLE_ACTIVE_HIGH == true)
#define DRIVER_ENABLE GPIO_PIN_SET
#define DRIVER_DISABLE GPIO_PIN_RESET
#else
#define DRIVER_ENABLE GPIO_PIN_RESET
#define DRIVER_DISABLE GPIO_PIN_SET
#endif

/**
 3 pwm bldc driver class
*/
class BLDCDriver3PWM: public BLDCDriver
{
  public:
    /**
      BLDCDriver class constructor
    */
    BLDCDriver3PWM(
      TIM_HandleTypeDef* phA_timer, uint32_t phA_channel,
      TIM_HandleTypeDef* phB_timer, uint32_t phB_channel,
      TIM_HandleTypeDef* phC_timer, uint32_t phC_channel,
      GPIO_TypeDef* enable_a_gpio_bank, uint16_t enable_a_pin,
      GPIO_TypeDef* enable_b_gpio_bank, uint16_t enable_b_pin,
      GPIO_TypeDef* enable_c_gpio_bank, uint16_t enable_c_pin
      );
    
    /**  Motor hardware init function */
  	int init() override;
    /** Motor disable function */
  	void disable() override;
    /** Motor enable function */
    void enable() override;

    // hardware variables
  	TIM_HandleTypeDef* _phA_timer, _phB_timer, _phC_timer;
    uint32_t _phA_channel, _phB_channel, _phC_channel;
    GPIO_TypeDef* _enable_a_gpio_bank, _enable_b_gpio_bank, _enable_c_gpio_bank;
    uint16_t _enable_a_pin, _enable_b_pin, _enable_c_pin;

    /** 
     * Set phase voltages to the harware 
     * 
     * @param Ua - phase A voltage
     * @param Ub - phase B voltage
     * @param Uc - phase C voltage
    */
    void setPwm(float Ua, float Ub, float Uc) override;

    /** 
     * Set phase voltages to the harware 
     * 
     * @param sc - phase A state : active / disabled ( high impedance )
     * @param sb - phase B state : active / disabled ( high impedance )
     * @param sa - phase C state : active / disabled ( high impedance )
    */
    virtual void setPhaseState(int sa, int sb, int sc) override;
  private:
};


#endif
