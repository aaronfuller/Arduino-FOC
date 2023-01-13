#ifndef MEGATRON_CW360_4220
#define MEGATRON_CW360_4220
#define VREF 3.3f
#define ADC_MAX 0x3FF
#define ENCODER_ADC_CHANNEL ADC_CHANNEL_12
#define INIT_MAX_V 0.8f
#define INIT_MIN_V 0.5f
#define ANGLE_CHANGE_THRESHOLD 0.005f

#include "../common/base_classes/Sensor.h"

extern "C" {
  #include "stm32g4xx_hal.h"
}

class megatron_cw360_4220: public Sensor{
    public:
        /**
         * Encoder class constructor
         */
        megatron_cw360_4220();
        void init(ADC_HandleTypeDef * hadc);
        float getSensorAngle() override;
        float last_measured_angle;
    
    private:
        /**
         * Function getting current angle register value
         * it uses angle_register variable
         */

        ADC_HandleTypeDef * _hadc;
        ADC_ChannelConfTypeDef _sConfig;
        uint16_t _adc_max;
        uint16_t _adc_min;
        uint16_t _adc_range;
};


#endif
