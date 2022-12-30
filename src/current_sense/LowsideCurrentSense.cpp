#include "LowsideCurrentSense.h"
// LowsideCurrentSensor constructor
//  - shunt_resistor  - shunt resistor value
//  - gain  - current-sense op-amp gain
//  - phA   - A phase adc pin
//  - phB   - B phase adc pin
//  - phC   - C phase adc pin (optional)

LowsideCurrentSense::LowsideCurrentSense(){
    
}

// Lowside sensor init function
int LowsideCurrentSense::init(float shunt_resistor, float gain, volatile uint16_t * adc_a, volatile uint16_t * adc_b, volatile uint16_t * adc_c){
    // configure ADC variables
    // params = _configureADCLowSide(driver->params,pinA,pinB,pinC);
    // // if init failed return fail
    // if (params == SIMPLEFOC_CURRENT_SENSE_INIT_FAILED) return 0; 
    // // sync the driver
    // _driverSyncLowSide(driver->params, params);
    // calibrate zero offsets
    this->_adc_a = adc_a;
    this->_adc_b = adc_b;
    this->_adc_c = adc_c;

    float volts_to_amps_ratio = 1.0f / shunt_resistor / gain; // volts to amps
    // gains for each phase
    this->gain_a = volts_to_amps_ratio;
    this->gain_b = volts_to_amps_ratio;
    this->gain_c = volts_to_amps_ratio;

    calibrateOffsets();
    // set the initialized flag
    // initialized = (params!=SIMPLEFOC_CURRENT_SENSE_INIT_FAILED);
    // return success
    return 1;
}
// Function finding zero offsets of the ADC
void LowsideCurrentSense::calibrateOffsets(){    
    const int calibration_rounds = 1000;

    // find adc offset = zero current voltage
    this->offset_ia = 0;
    this->offset_ib = 0;
    this->offset_ic = 0;
    // read the adc voltage 1000 times ( arbitrary number )
    for (int i = 0; i < calibration_rounds; i++) {
        // _startADC3PinConversionLowSide();
        this->offset_ia += (((float)(*(this->_adc_a)) / (float)ADC_MAX) * VREF);
        this->offset_ib += (((float)(*(this->_adc_b)) / (float)ADC_MAX) * VREF);
        this->offset_ic += (((float)(*(this->_adc_c)) / (float)ADC_MAX) * VREF);
        HAL_Delay(1);
    }
    // calculate the mean offsets
    this->offset_ia = this->offset_ia / calibration_rounds;
    this->offset_ib = this->offset_ib / calibration_rounds;
    this->offset_ic = this->offset_ic / calibration_rounds;
}

// read all three phase currents (if possible 2 or 3)
PhaseCurrent_s LowsideCurrentSense::getPhaseCurrents(){
    PhaseCurrent_s current;
    // _startADC3PinConversionLowSide();
    current.a = ((((float)(*(this->_adc_a)) / (float)ADC_MAX) * VREF) - this->offset_ia) * this->gain_a;// amps
    current.b = ((((float)(*(this->_adc_b)) / (float)ADC_MAX) * VREF) - this->offset_ib) * this->gain_b;// amps
    current.c = ((((float)(*(this->_adc_b)) / (float)ADC_MAX) * VREF) - this->offset_ic) * this->gain_c; // amps
    return current;
}

// Function aligning the current sense with motor driver
// if all pins are connected well none of this is really necessary! - can be avoided
// returns flag
// 0 - fail
// 1 - success and nothing changed
// 2 - success but pins reconfigured
// 3 - success but gains inverted
// 4 - success but pins reconfigured and gains inverted
int LowsideCurrentSense::driverAlign(float voltage){
    return 1;
}
