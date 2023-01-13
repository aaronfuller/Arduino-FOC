#include "BLDCDriver3PWM.h"

BLDCDriver3PWM::BLDCDriver3PWM() {
  // default power-supply value
  this->voltage_power_supply = DEF_POWER_SUPPLY;
  this->voltage_limit = NOT_SET;
  this->pwm_frequency = NOT_SET;
}

// init hardware pins
int BLDCDriver3PWM::init(
  TIM_HandleTypeDef* phA_timer, uint32_t phA_channel,
  TIM_HandleTypeDef* phB_timer, uint32_t phB_channel,
  TIM_HandleTypeDef* phC_timer, uint32_t phC_channel,
  GPIO_TypeDef* enable_a_gpio_bank, uint16_t enable_a_pin,
  GPIO_TypeDef* enable_b_gpio_bank, uint16_t enable_b_pin,
  GPIO_TypeDef* enable_c_gpio_bank, uint16_t enable_c_pin
  ) 
{
  // Pin initialization
  this->_phA_timer = phA_timer;
  this->_phB_timer = phB_timer;
  this->_phC_timer = phC_timer;

  this->_phA_channel = phA_channel;
  this->_phB_channel = phB_channel;
  this->_phC_channel = phC_channel;

  // enable_pin pin
  this->_enable_a_gpio_bank = enable_a_gpio_bank;
  this->_enable_b_gpio_bank = enable_b_gpio_bank;
  this->_enable_c_gpio_bank = enable_c_gpio_bank;

  this->_enable_a_pin = enable_a_pin;
  this->_enable_b_pin = enable_b_pin;
  this->_enable_c_pin = enable_c_pin;

  // sanity check for the voltage limit configuration
  if(!_isset(voltage_limit) || voltage_limit > voltage_power_supply) voltage_limit = voltage_power_supply;
  this->initialized = true;

  return true;
}

// enable motor driver
void  BLDCDriver3PWM::enable(){
  HAL_GPIO_WritePin(this->_enable_a_gpio_bank, this->_enable_a_pin, DRIVER_ENABLE);
  HAL_GPIO_WritePin(this->_enable_b_gpio_bank, this->_enable_b_pin, DRIVER_ENABLE);
  HAL_GPIO_WritePin(this->_enable_c_gpio_bank, this->_enable_c_pin, DRIVER_ENABLE);
  // set zero to PWM
  setPwm(0,0,0);
}

// disable motor driver
void BLDCDriver3PWM::disable()
{
  // set zero to PWM
  setPwm(0, 0, 0);
  // disable the driver - if enable_pin pin available
  HAL_GPIO_WritePin(this->_enable_a_gpio_bank, this->_enable_a_pin, DRIVER_DISABLE);
  HAL_GPIO_WritePin(this->_enable_b_gpio_bank, this->_enable_b_pin, DRIVER_DISABLE);
  HAL_GPIO_WritePin(this->_enable_c_gpio_bank, this->_enable_c_pin, DRIVER_DISABLE);
}

// Set voltage to the pwm pin
void BLDCDriver3PWM::setPhaseState(int sa, int sb, int sc) {
  // disable if needed
  HAL_GPIO_WritePin(this->_enable_a_gpio_bank, this->_enable_a_pin, sa == _HIGH_IMPEDANCE ? DRIVER_DISABLE : DRIVER_ENABLE);
  HAL_GPIO_WritePin(this->_enable_b_gpio_bank, this->_enable_b_pin, sb == _HIGH_IMPEDANCE ? DRIVER_DISABLE : DRIVER_ENABLE);
  HAL_GPIO_WritePin(this->_enable_c_gpio_bank, this->_enable_c_pin, sc == _HIGH_IMPEDANCE ? DRIVER_DISABLE : DRIVER_ENABLE);
}

// Set voltage to the pwm pin
void BLDCDriver3PWM::setPwm(float Ua, float Ub, float Uc) {
  float dc_a, dc_b, dc_c;

  // limit the voltage in driver
  Ua = _constrain(Ua, 0.0f, voltage_limit);
  Ub = _constrain(Ub, 0.0f, voltage_limit);
  Uc = _constrain(Uc, 0.0f, voltage_limit);
  // calculate duty cycle
  // limited in [0,1]
  dc_a = _constrain(Ua / voltage_power_supply, 0.0f , 1.0f );
  dc_b = _constrain(Ub / voltage_power_supply, 0.0f , 1.0f );
  dc_c = _constrain(Uc / voltage_power_supply, 0.0f , 1.0f );
  // dc_a = Ua / voltage_power_supply;
  // dc_b = Ub / voltage_power_supply;
  // dc_c = Uc / voltage_power_supply;

  // hardware specific writing
  // hardware specific function - depending on driver and mcu
  __HAL_TIM_SET_COMPARE(this->_phA_timer, this->_phA_channel, (uint32_t)(dc_a*ARR_VALUE));
  __HAL_TIM_SET_COMPARE(this->_phB_timer, this->_phB_channel, (uint32_t)(dc_b*ARR_VALUE));
  __HAL_TIM_SET_COMPARE(this->_phC_timer, this->_phC_channel, (uint32_t)(dc_c*ARR_VALUE));
}
