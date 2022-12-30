#ifndef MEGATRON_CW360_4220
#define MEGATRON_CW360_4220
#define VREF 3.3f
#define ADC_MAX 0xFFF
#define INIT_MAX_V 2.55f
#define INIT_MIN_V 0.60f

#include "../common/base_classes/Sensor.h"

class megatron_cw360_4220: public Sensor{
    public:
        /**
         * Encoder class constructor
         */
        megatron_cw360_4220();
        void init(volatile uint16_t * adc_val);
        float getSensorAngle() override;
    
    private:
        /**
         * Function getting current angle register value
         * it uses angle_register variable
         */
        volatile uint16_t * _adc_val;
        uint16_t _adc_max;
        uint16_t _adc_min;
        uint16_t _adc_range;
};


#endif
