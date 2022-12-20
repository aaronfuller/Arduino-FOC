#ifndef MEGATRON_CW360_4220
#define MEGATRON_CW360_4220
#define ADC_BUF_LEN 1
#define VREF 3.3f
#define INIT_MAX_V 2.55f
#define INIT_MIN_V 0.60f

#include "../common/base_classes/Sensor.h"
#include "main.h"

class megatron_cw360_4220: public Sensor{
    public:
        /**
         * Encoder class constructor
         */
        megatron_cw360_4220(ADC_HandleTypeDef _adc_handle);
        void init();
        float getSensorAngle() override;
    
        ADC_HandleTypeDef _adc_handle;

    private:
        /**
         * Function getting current angle register value
         * it uses angle_register variable
         */
        uint16_t _adc_buf[ADC_BUF_LEN];
        uint16_t _adc_max;
        uint16_t _adc_min;
        uint16_t _adc_range;

};


#endif
