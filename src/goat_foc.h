#ifndef GOAT_FOC_H
#define GOAT_FOC_H

#define BATTERY_VOLTAGE 11.3f

#include "stm32g4xx_hal.h"
// #include "stm32g4xx_hal_tim.h"
// #include "stm32g4xx_hal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

void initialize_driver(
    TIM_HandleTypeDef* phA_timer, uint32_t phA_channel,
    TIM_HandleTypeDef* phB_timer, uint32_t phB_channel,
    TIM_HandleTypeDef* phC_timer, uint32_t phC_channel,
    TIM_HandleTypeDef* adc_timer_1, uint32_t adc_tim_channel_1,
    TIM_HandleTypeDef* adc_timer_2, uint32_t adc_tim_channel_2,
    GPIO_TypeDef* enable_a_gpio_bank, uint16_t enable_a_pin,
    GPIO_TypeDef* enable_b_gpio_bank, uint16_t enable_b_pin,
    GPIO_TypeDef* enable_c_gpio_bank, uint16_t enable_c_pin
);
void initialize_encoder(ADC_HandleTypeDef * hadc);
void initialize_current_sense(
    float shunt_resistor, 
    float gain, 
    ADC_HandleTypeDef * hadc_1, 
    ADC_HandleTypeDef * hadc_2);
int initialize_motor(int pole_pairs, float phase_resistance, float kv);


void loop_goat_foc();

#ifdef __cplusplus
}
#endif


#endif
