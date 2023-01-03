#ifndef MEGATRON_CW360_4220
#define MEGATRON_CW360_4220
#define VREF 3.3f
#define ADC_MAX 0xFFF
#define INIT_MAX_V 1.85f
#define INIT_MIN_V 0.90f

#include "../common/base_classes/Sensor.h"

class megatron_cw360_4220: public Sensor{
    public:
        /**
         * Encoder class constructor
         */
        megatron_cw360_4220();
        void init(volatile uint16_t * adc_val, volatile uint32_t * adc_tick_updated);
        float getSensorAngle() override;
        float last_measured_angle;
    
    private:
        /**
         * Function getting current angle register value
         * it uses angle_register variable
         */
        volatile uint32_t * _adc_tick_updated;
        uint32_t _last_adc_tick_updated;

        volatile uint16_t * _adc_val;
        uint16_t _adc_max;
        uint16_t _adc_min;
        uint16_t _adc_range;
};


#endif
