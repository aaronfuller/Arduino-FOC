#include "../common/foc_utils.h"
#include "../common/time_utils.h"
#include "megatron_cw360_4220.h"

/** MagneticSensorAnalog(ADC_HandleTypeDef adc_handle)
 * @param adc_handle adc handle for sensor
 */
megatron_cw360_4220::megatron_cw360_4220(ADC_HandleTypeDef adc_handle) {
    this->_adc_handle = adc_handle;
}


void megatron_cw360_4220::init(){
    HAL_ADC_Start_DMA(&(this->_adc_handle), (uint32_t*)(this->_adc_buf), ADC_BUF_LEN);
    
    // Start max below probable minimum value for the max
    this->_adc_max = (uint16_t)(INIT_MAX_V / VREF) * (0xFFF);
    // Start min above probable max value for the min
    this->_adc_min = (uint16_t)(INIT_MIN_V / VREF) * (0xFFF);
    this->_adc_range = _adc_max - _adc_min;

    this->Sensor::init(); // call base class init
}

//  Shaft angle calculation
//  angle is in radians [rad]
float megatron_cw360_4220::getSensorAngle(){
    // raw data from the sensor
    uint16_t buf_val, buf_val_less_offset;
    buf_val = this->_adc_buf[0];

    if (buf_val > this->_adc_max) {
        this->_adc_max = buf_val;
        this->_adc_range = _adc_max - _adc_min;
    } else if (buf_val < this->_adc_min) {
        this->_adc_min = buf_val;
        this->_adc_range = _adc_max - _adc_min;
    }

    buf_val_less_offset = buf_val - this->_adc_min;
    return ((float)(buf_val_less_offset) / (float)(this->_adc_range)) * _2PI;
}
