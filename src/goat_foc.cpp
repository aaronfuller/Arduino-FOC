#include "goat_foc.h"
#include <memory>

#include "BLDCMotor.h"
#include "sensors/megatron_cw360_4220.h"
#include "drivers/BLDCDriver3PWM.h"
#include "current_sense/LowsideCurrentSense.h"

BLDCDriver3PWM driver;
megatron_cw360_4220 sensor;
LowsideCurrentSense current_sense;
std::unique_ptr<BLDCMotor> motor;

int driver_initialized = 0;
int sensor_initialized = 0;
int current_sense_initialized = 0;

// C wrapper to initialize driver object
void initialize_driver(
    TIM_HandleTypeDef* phA_timer, uint32_t phA_channel,
    TIM_HandleTypeDef* phB_timer, uint32_t phB_channel,
    TIM_HandleTypeDef* phC_timer, uint32_t phC_channel,
    GPIO_TypeDef* enable_a_gpio_bank, uint16_t enable_a_pin,
    GPIO_TypeDef* enable_b_gpio_bank, uint16_t enable_b_pin,
    GPIO_TypeDef* enable_c_gpio_bank, uint16_t enable_c_pin
    ) 
{
    driver.voltage_power_supply = BATTERY_VOLTAGE;
    driver.pwm_frequency = 36603.22;
    driver.init(
        phA_timer, phA_channel,
        phB_timer, phB_channel,
        phC_timer, phC_channel,
        enable_a_gpio_bank, enable_a_pin,
        enable_b_gpio_bank, enable_b_pin,
        enable_c_gpio_bank, enable_c_pin
        );
    driver_initialized = 1;
    if (current_sense_initialized) current_sense.linkDriver(&driver);
}

// C wrapper to initialize megatron encoder
void initialize_encoder(volatile uint16_t * adc_val, volatile uint32_t * adc_tick_updated) {
    sensor.init(adc_val, adc_tick_updated);
    sensor_initialized = 1;
}

// C wrapper to initialize current sense hardware
// void initialize_current_sense(float shunt_resistor, float gain, volatile uint16_t * adc_a, volatile uint16_t * adc_b, volatile uint16_t * adc_c, volatile uint32_t * adc_1_tick_updated, volatile uint32_t * adc_2_tick_updated) {
//     current_sense.init(shunt_resistor, gain, adc_a, adc_b, adc_c, adc_1_tick_updated, adc_2_tick_updated);
//     if (driver_initialized) current_sense.linkDriver(&driver);
//     current_sense_initialized = 1;
// }

// C wrapper to initialize the motor
int initialize_motor(int pole_pairs, float phase_resistance, float kv) {
    // create motor object
    motor = std::make_unique<BLDCMotor>(pole_pairs, phase_resistance, kv*1.0f);

    // set some motor settings

    // link hardware
    if (driver_initialized && sensor_initialized && current_sense_initialized) {
        motor->linkDriver(&driver);
        motor->linkSensor(&sensor);
        // motor->linkCurrentSense(&current_sense);
    } else return 1;


    motor->motion_downsample = 10;

    motor->controller = MotionControlType::velocity;
    motor->PID_velocity.P = 0.05;
    motor->PID_velocity.I = 5;
    motor->PID_velocity.D = 0.001;
    motor->PID_velocity.output_ramp = 1000;
    motor->LPF_velocity.Tf = 0.05;

    motor->current_limit = 44.0f;
    motor->torque_controller = TorqueControlType::voltage;
    motor->voltage_limit = 1.1f;
    // motor->foc_modulation = FOCModulationType::Trapezoid_120;

    // initialize motor
    motor->init();
    motor->initFOC();

    // return success
    return 0;
}

// C wrapper to call the FOC loop
void loop_goat_foc() {
    motor->loopFOC();
    // sensor.getSensorAngle();
    motor->move(5);
}