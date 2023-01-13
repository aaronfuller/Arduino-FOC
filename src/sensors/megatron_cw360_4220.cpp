#include "../common/foc_utils.h"
#include "../common/time_utils.h"
#include "megatron_cw360_4220.h"

/** MagneticSensorAnalog(volatile uint16_t * adc_val)
 * @param adc_val adc value for sensor
 */
megatron_cw360_4220::megatron_cw360_4220() {
    
}

void megatron_cw360_4220::init(ADC_HandleTypeDef * hadc){
    this->_hadc = hadc;
    // Start max below probable minimum value for the max
    // this->_adc_max = (uint16_t)((INIT_MAX_V / VREF) * (ADC_MAX));
    this->_adc_max = 459;
    // Start min above probable max value for the min
    // this->_adc_min = (uint16_t)((INIT_MIN_V / VREF) * (ADC_MAX));
    this->_adc_min = 92;
    this->_adc_range = _adc_max - _adc_min;

    this->_sConfig.Channel = ENCODER_ADC_CHANNEL;
    this->_sConfig.Rank = ADC_REGULAR_RANK_1;
    this->_sConfig.SamplingTime = ADC_SAMPLETIME_92CYCLES_5;
    this->_sConfig.SingleDiff = ADC_SINGLE_ENDED;
    this->_sConfig.OffsetNumber = ADC_OFFSET_NONE;
    this->_sConfig.Offset = 0;

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
    // raw data from the sensor
    int num_samples = 4;
    float angle_measured;
    uint32_t buf_val = 0;
    uint32_t buf_val_less_offset = 0;

    if (HAL_ADC_ConfigChannel(this->_hadc, &(this->_sConfig)) != HAL_OK) {
        // Error_Handler();
    }
    for (int count = 0; count < num_samples; count++) {
        HAL_ADC_Start(this->_hadc);
        HAL_ADC_PollForConversion(this->_hadc, HAL_MAX_DELAY);
        buf_val += HAL_ADC_GetValue(this->_hadc);
        HAL_ADC_Stop(this->_hadc);
    }
    buf_val /= num_samples;
    if (buf_val > this->_adc_max) {
        this->_adc_max = buf_val;
        this->_adc_range = _adc_max - _adc_min;
    } else if (buf_val < this->_adc_min) {
        this->_adc_min = buf_val;
        this->_adc_range = _adc_max - _adc_min;
    }

    buf_val_less_offset = buf_val - this->_adc_min;
    angle_measured = ((float)(buf_val_less_offset) / (float)(this->_adc_range)) * _2PI;
    angle_measured = (int)(angle_measured * 50 + .5);
    angle_measured = (float)angle_measured / 50;

    // if (fabs(this->last_measured_angle - angle_measured) < ANGLE_CHANGE_THRESHOLD)
    //     return this->last_measured_angle;

    this->last_measured_angle = angle_measured;
    return angle_measured;
}
