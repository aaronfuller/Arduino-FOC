#include "../common/foc_utils.h"
#include "../common/time_utils.h"
#include "megatron_cw360_4220.h"

/** MagneticSensorAnalog(volatile uint16_t * adc_val)
 * @param adc_val adc value for sensor
 */
megatron_cw360_4220::megatron_cw360_4220() {
    
}

void megatron_cw360_4220::init(volatile uint16_t * adc_val, volatile uint32_t * adc_tick_updated){
    this->_adc_tick_updated = adc_tick_updated;
    this->_last_adc_tick_updated = 0;
    
    this->_adc_val = adc_val;
    // Start max below probable minimum value for the max
    this->_adc_max = (uint16_t)((INIT_MAX_V / VREF) * (ADC_MAX));
    // Start min above probable max value for the min
    this->_adc_min = (uint16_t)((INIT_MIN_V / VREF) * (ADC_MAX));
    this->_adc_range = _adc_max - _adc_min;

    // initialize all the internal variables of Sensor to ensure a "smooth" startup (without a 'jump' from zero)
    getSensorAngle(); // call once
    _delay(1);
    vel_angle_prev = getSensorAngle(); // call again
    vel_angle_prev_ts = _micros();
    _delay(1);
    this->last_measured_angle = getSensorAngle(); // call once
    _delay(1);
    angle_prev = getSensorAngle(); // call again
    angle_prev_ts = _micros();
}

//  Shaft angle calculation
//  angle is in radians [rad]
float megatron_cw360_4220::getSensorAngle(){
    // wait until new value from DMA
    while (*(this->_adc_tick_updated) == (this->_last_adc_tick_updated));

    // raw data from the sensor
    float angle_measured;
    uint16_t buf_val, buf_val_less_offset;
    buf_val = *(this->_adc_val);

    if (buf_val > this->_adc_max) {
        this->_adc_max = buf_val;
        this->_adc_range = _adc_max - _adc_min;
    } else if (buf_val < this->_adc_min) {
        this->_adc_min = buf_val;
        this->_adc_range = _adc_max - _adc_min;
    }

    buf_val_less_offset = buf_val - this->_adc_min;
    angle_measured = ((float)(buf_val_less_offset) / (float)(this->_adc_range)) * _2PI;
    this->last_measured_angle = angle_measured * 57.2958;

    this->_last_adc_tick_updated = *(this->_adc_tick_updated);  
    
    return angle_measured;
}
