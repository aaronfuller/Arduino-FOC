#include "goat_foc.h"
#include <memory>
#include <cstring>

#include "BLDCMotor.h"
#include "sensors/megatron_cw360_4220.h"
#include "drivers/BLDCDriver3PWM.h"
#include "current_sense/LowsideCurrentSense.h"
#include "communication/Commander.h"

BLDCDriver3PWM driver;
megatron_cw360_4220 sensor;
LowsideCurrentSense current_sense;
std::unique_ptr<BLDCMotor> motor;
std::unique_ptr<Commander> commander;
std::unique_ptr<Print> printer;
char * incoming_ethernet_command;

int motor_initialized = 0;
int driver_initialized = 0;
int sensor_initialized = 0;
int current_sense_initialized = 0;

// C wrapper to initialize driver object
void initialize_driver(
    TIM_HandleTypeDef* phA_timer, uint32_t phA_channel,
    TIM_HandleTypeDef* phB_timer, uint32_t phB_channel,
    TIM_HandleTypeDef* phC_timer, uint32_t phC_channel,
    TIM_HandleTypeDef* adc_timer_1, uint32_t adc_tim_channel_1,
    TIM_HandleTypeDef* adc_timer_2, uint32_t adc_tim_channel_2,
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
        adc_timer_1, adc_tim_channel_1,
        adc_timer_2, adc_tim_channel_2,
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
void initialize_current_sense(float shunt_resistor, float gain, ADC_HandleTypeDef * hadc_1, ADC_HandleTypeDef * hadc_2) {
    current_sense.init(shunt_resistor, gain, hadc_1, hadc_2);
    if (driver_initialized) current_sense.linkDriver(&driver);
    current_sense_initialized = 1;
}

// C wrapper to initialize the motor
int initialize_motor(int pole_pairs, float phase_resistance, float kv) {
    // create motor object
    motor = std::make_unique<BLDCMotor>(pole_pairs, phase_resistance, kv*1.0f);

    // link hardware
    if (driver_initialized && sensor_initialized && current_sense_initialized) {
        motor->linkDriver(&driver);
        motor->linkSensor(&sensor);
        motor->linkCurrentSense(&current_sense);
    } else return 1;
    

    // motor->motion_downsample = 10;

    motor->controller = MotionControlType::torque;

    // motor->PID_velocity.P = 0.375;
    // motor->PID_velocity.P = 0.1;
    // motor->PID_velocity.P = 2.7;
    // motor->PID_velocity.I = 0.3;
    // motor->PID_velocity.I = 0.01;
    // motor->PID_velocity.I = 34.46798;
    // motor->PID_velocity.D = 0.0000001;
    // motor->PID_velocity.D = 0.0005;
    // motor->PID_velocity.D = 0.05287515;

    // motor->PID_velocity.output_ramp = 10000;
    motor->LPF_velocity.Tf = 0.075;

    motor->current_limit = 5.0f;
    motor->torque_controller = TorqueControlType::foc_current;
    motor->voltage_sensor_align = 1.0f;
    motor->voltage_limit = 1.5f;
    motor->foc_modulation = FOCModulationType::SpaceVectorPWM;

    // motor->velocity_limit = 0.1;


    // 91.5 spins

    // float offset = float(3.14159265359 / 100.0) * 97.5;  //where guess = somewhere between 0 to 200.
    // Direction direction = Direction::CCW;

    // initialize motor
    motor->init();
    // motor->initFOC(offset, direction);
    motor->initFOC();

    motor->target = 1;

    // return success
    motor_initialized = 1;
    return 0;
}

void doMotor(char * cmd) {
    commander->motor(motor.get(), cmd);
}

int init_comms(char * command_buffer, int (*write_wrapper)(const char *, size_t)) {
    if (!motor_initialized) return 1;
    commander = std::make_unique<Commander>('\n', false);
    commander->add('M', doMotor, "motor");
    incoming_ethernet_command = command_buffer;

    printer = std::make_unique<Print>(write_wrapper);
    motor->useMonitoring(*(printer.get()));
    motor->monitor_variables = _MON_TARGET | _MON_VOLT_Q | _MON_VEL | _MON_ANGLE;
    motor->monitor_downsample = 100; // default 10
    // SimpleFOCDebug::enable(NULL);

    return 0;
}

float vel = 0;
float motor_target = 0;
uint32_t start_loop;
uint32_t end_loop;
uint32_t loop_duration;
float angle_diff;
float current_angle = 0;
float prev_angle = 0;
PhaseCurrent_s currents;
float a_current = 0;
float b_current = 0;
float c_current = 0;
char * current_command = nullptr;
// int count;
// int position = 0;
// C wrapper to call the FOC loop
void loop_goat_foc() {
    start_loop = _micros();

    motor->loopFOC();
    motor->move();
    motor->monitor();

    current_command = incoming_ethernet_command;
    while (current_command[0] != 0) {
        commander->run(current_command);
        current_command = strchr(current_command, '\n') + 1;
    }
    
    vel = motor->shaftVelocity();
    motor_target = motor->target;


    end_loop = _micros();
    loop_duration = end_loop - start_loop;

    // a_current = motor->current.q;
    // b_current = motor->current.d;
    currents = current_sense.getPhaseCurrents();
    a_current = currents.a;
    b_current = currents.b;
    c_current = currents.c;

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