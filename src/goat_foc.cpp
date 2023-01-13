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

// REMOVE - FOR DEBUGGING
int xts_index_lookup[] = {1, 2, 0};

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
    driver.pwm_frequency = 150000000.0f / 2048.0f;
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
void initialize_encoder(ADC_HandleTypeDef * hadc) {
    sensor.init(hadc);
    sensor_initialized = 1;
}

// C wrapper to initialize current sense hardware
void initialize_current_sense(float shunt_resistor, float gain, volatile uint16_t * adc_a, volatile uint16_t * adc_b, volatile uint16_t * adc_c, volatile uint32_t * adc_1_tick_updated, volatile uint32_t * adc_2_tick_updated) {
    current_sense.init(shunt_resistor, gain, adc_a, adc_b, adc_c, adc_1_tick_updated, adc_2_tick_updated);
    if (driver_initialized) current_sense.linkDriver(&driver);
    current_sense_initialized = 1;
}

// C wrapper to initialize the motor
int initialize_motor(int pole_pairs, float phase_resistance, float kv) {
    // create motor object
    motor = std::make_unique<BLDCMotor>(pole_pairs, phase_resistance, kv*1.0f);

    // set some motor settings

    // link hardware
    motor->linkDriver(&driver);
    motor->linkSensor(&sensor);
    // if (driver_initialized && sensor_initialized && current_sense_initialized) {
    //     // motor->linkCurrentSense(&current_sense);
    // } else return 1;
    

    // motor->motion_downsample = 10;

    motor->controller = MotionControlType::velocity_openloop;

    // motor->PID_velocity.P = 0.375;
    motor->PID_velocity.P = 1;
    // motor->PID_velocity.I = 0.3;
    motor->PID_velocity.I = 0;
    // motor->PID_velocity.D = 0.0000001;
    motor->PID_velocity.D = 0;

    motor->PID_velocity.output_ramp = 10000;
    motor->LPF_velocity.Tf = 0;

    motor->current_limit = 30.0f;
    motor->torque_controller = TorqueControlType::voltage;
    motor->voltage_sensor_align = 1.0f;
    motor->voltage_limit = 1.0f;
    // motor->foc_modulation = FOCModulationType::Trapezoid_150;

    // motor->velocity_limit = 0.1;

    float offset = float(3.14159265359 / 100.0) * 91.5;  //where guess = somewhere between 0 to 200.
    Direction direction = Direction::CCW;

    // initialize motor
    motor->init();
    // motor->initFOC(offset, direction);
    motor->initFOC();

    // motor->target = 1;

    // return success
    return 0;
}

float vel = 0;
uint32_t start_loop;
uint32_t end_loop;
uint32_t loop_duration;
float angle_diff;
float current_angle = 0;
float prev_angle = 0;
// int count;
// int position = 0;
// C wrapper to call the FOC loop
void loop_goat_foc() {
    start_loop = _micros();
    motor->loopFOC();
    end_loop = _micros();
    loop_duration = end_loop - start_loop;
    // sensor.update();
    motor->move(15);

    vel = motor->shaftVelocity();

    // prev_angle = current_angle;
    // current_angle = sensor.getMechanicalAngle();

    

    // angle_diff = fmod(current_angle, (_2PI / 2));
    // if (count == 5000) {
    //     count = 0;
    //     position++;
    //     if (position == 7)
    //         position = 0;
    // }
    // count++;
}