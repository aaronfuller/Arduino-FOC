#include "BLDCDriver3PWM.h"

BLDCDriver3PWM::BLDCDriver3PWM(
  TIM_HandleTypeDef* phA_timer, uint32_t phA_channel,
  TIM_HandleTypeDef* phB_timer, uint32_t phB_channel,
  TIM_HandleTypeDef* phC_timer, uint32_t phC_channel,
  GPIO_TypeDef* enable_a_gpio_bank, uint16_t enable_a_pin,
  GPIO_TypeDef* enable_b_gpio_bank, uint16_t enable_b_pin,
  GPIO_TypeDef* enable_c_gpio_bank, uint16_t enable_c_pin
  );

  // Pin initialization
  _phA_timer = phA_timer;
  _phB_timer = phB_timer;
  _phC_timer = phC_timer;

  _phA_channel = phA_channel;
  _phB_channel = phB_channel;
  _phC_channel = phC_channel;

  // enable_pin pin
  _enable_a_gpio_bank = enable_a_gpio_bank;
  _enable_b_gpio_bank = enable_b_gpio_bank;
  _enable_b_gpio_bank = enable_b_gpio_bank;

  _enable_a_pin = enable_a_pin;
  _enable_b_pin = enable_b_pin;
  _enable_b_pin = enable_b_pin;

  // default power-supply value
  voltage_power_supply = DEF_POWER_SUPPLY;
  voltage_limit = NOT_SET;
  pwm_frequency = NOT_SET;
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

// init hardware pins
int BLDCDriver3PWM::init() {
  // sanity check for the voltage limit configuration
  if(!_isset(voltage_limit) || voltage_limit > voltage_power_supply) voltage_limit =  voltage_power_supply;
  return true;
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

  // limit the voltage in driver
  Ua = _constrain(Ua, 0.0f, voltage_limit);
  Ub = _constrain(Ub, 0.0f, voltage_limit);
  Uc = _constrain(Uc, 0.0f, voltage_limit);
  // calculate duty cycle
  // limited in [0,1]
  dc_a = _constrain(Ua / voltage_power_supply, 0.0f , 1.0f );
  dc_b = _constrain(Ub / voltage_power_supply, 0.0f , 1.0f );
  dc_c = _constrain(Uc / voltage_power_supply, 0.0f , 1.0f );

  // hardware specific writing
  // hardware specific function - depending on driver and mcu
  __HAL_TIM_SET_COMPARE(_phA_timer, _phA_channel, (int)(dc_a*ARR_VALUE));
  __HAL_TIM_SET_COMPARE(_phB_timer, _phB_channel, (int)(dc_b*ARR_VALUE));
  __HAL_TIM_SET_COMPARE(_phC_timer, _phC_channel, (int)(dc_c*ARR_VALUE));
}
