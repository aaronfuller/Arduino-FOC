#ifndef LOWSIDE_CS_LIB_H
#define LOWSIDE_CS_LIB_H

#include "../common/foc_utils.h"
#include "../common/time_utils.h"
#include "../common/defaults.h"
#include "../common/base_classes/CurrentSense.h"
#include "../common/base_classes/FOCMotor.h"
#include "../common/lowpass_filter.h"

extern "C" {
  #include "stm32g4xx_hal.h"
}

#define VREF 3.3f
#define ADC_MAX 0x3FF
#define PHASE_A_CHANNEL ADC_CHANNEL_3
#define PHASE_B_CHANNEL ADC_CHANNEL_4
#define PHASE_C_CHANNEL ADC_CHANNEL_17

class LowsideCurrentSense: public CurrentSense{
  public:
    /**
      LowsideCurrentSense class constructor
      @param shunt_resistor shunt resistor value
      @param gain current-sense op-amp gain
      @param phA A phase adc pin
      @param phB B phase adc pin
      @param phC C phase adc pin (optional)
    */
    LowsideCurrentSense();

    // CurrentSense interface implementing functions
    int init(float shunt_resistor, float gain, ADC_HandleTypeDef * hadc_1, ADC_HandleTypeDef * hadc_2);
    PhaseCurrent_s getPhaseCurrents() override;
    int driverAlign(float align_voltage) override;

    // ADC measuremnet gain for each phase
    // support for different gains for different phases of more commonly - inverted phase currents
    // this should be automated later
    float gain_a; //!< phase A gain
    float gain_b; //!< phase B gain
    float gain_c; //!< phase C gain

    // // per phase low pass fileters
    LowPassFilter lpf_a{DEF_LPF_PER_PHASE_CURRENT_SENSE_Tf}; //!<  current A low pass filter
    // LowPassFilter lpf_b{DEF_LPF_PER_PHASE_CURRENT_SENSE_Tf}; //!<  current B low pass filter
    LowPassFilter lpf_c{DEF_LPF_PER_PHASE_CURRENT_SENSE_Tf}; //!<  current C low pass filter
    LowPassFilter lpf_b{DEF_LPF_PER_PHASE_CURRENT_SENSE_Tf};

    float offset_ia; //!< zero current A voltage value (center of the adc reading)
    float offset_ib; //!< zero current B voltage value (center of the adc reading)
    float offset_ic; //!< zero current C voltage value (center of the adc reading)
  private:
    ADC_HandleTypeDef * _hadc_1;
    ADC_HandleTypeDef * _hadc_2;

    ADC_ChannelConfTypeDef _sConfig_a;
    ADC_ChannelConfTypeDef _sConfig_b;
    ADC_ChannelConfTypeDef _sConfig_c;

    /**
     *  Function finding zero offsets of the ADC
     */
    void calibrateOffsets();

};

#endif
