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
    GPIO_TypeDef* enable_a_gpio_bank, uint16_t enable_a_pin,
    GPIO_TypeDef* enable_b_gpio_bank, uint16_t enable_b_pin,
    GPIO_TypeDef* enable_c_gpio_bank, uint16_t enable_c_pin
    );
void initialize_encoder(volatile uint16_t * adc_val);
void initialize_current_sense(float shunt_resistor, float gain, volatile uint16_t * adc_a, volatile uint16_t * adc_b, volatile uint16_t * adc_c);
int initialize_motor(int pole_pairs, float phase_resistance, float kv);


void loop_goat_foc();

#ifdef __cplusplus
}
#endif


#endif
