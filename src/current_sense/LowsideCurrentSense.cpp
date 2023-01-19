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
int LowsideCurrentSense::init(float shunt_resistor, float gain, ADC_HandleTypeDef * hadc_1, ADC_HandleTypeDef * hadc_2){
    // configure ADC variables
    // params = _configureADCLowSide(driver->params,pinA,pinB,pinC);
    // // if init failed return fail
    // if (params == SIMPLEFOC_CURRENT_SENSE_INIT_FAILED) return 0; 
    // // sync the driver
    // _driverSyncLowSide(driver->params, params);
    // calibrate zero offsets

    this->_hadc_1 = hadc_1;
    this->_hadc_2 = hadc_2;

    // Phase A config
    // PA2, ADC1, Channel 3
    this->_sConfigInjected_a.InjectedChannel = PHASE_A_CHANNEL;
    this->_sConfigInjected_a.InjectedRank = ADC_REGULAR_RANK_1;
    this->_sConfigInjected_a.InjectedSamplingTime = ADC_SAMPLETIME_92CYCLES_5;
    this->_sConfigInjected_a.InjectedSingleDiff = ADC_SINGLE_ENDED;
    this->_sConfigInjected_a.InjectedOffsetNumber = ADC_OFFSET_NONE;
    this->_sConfigInjected_a.InjectedOffset = 0;
    this->_sConfigInjected_a.InjectedNbrOfConversion = 1;
    this->_sConfigInjected_a.InjectedDiscontinuousConvMode = DISABLE;
    this->_sConfigInjected_a.AutoInjectedConv = DISABLE;
    this->_sConfigInjected_a.QueueInjectedContext = DISABLE;
    // this->_sConfigInjected_a.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJEC_T1_TRGO2;
    this->_sConfigInjected_a.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
    // this->_sConfigInjected_a.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_RISING;
    this->_sConfigInjected_a.InjecOversamplingMode = DISABLE;
    // this->_sConfigInjected_a.InjecOversampling.Ratio = ADC_OVERSAMPLING_RATIO_2;
    // this->_sConfigInjected_a.InjecOversampling.RightBitShift = ADC_RIGHTBITSHIFT_3;

    // Phase B config
    // PA3, ADC1, Channel 4
    this->_sConfigInjected_b.InjectedChannel = PHASE_B_CHANNEL;
    this->_sConfigInjected_b.InjectedRank = ADC_REGULAR_RANK_1;
    this->_sConfigInjected_b.InjectedSamplingTime = ADC_SAMPLETIME_92CYCLES_5;
    this->_sConfigInjected_b.InjectedSingleDiff = ADC_SINGLE_ENDED;
    this->_sConfigInjected_b.InjectedOffsetNumber = ADC_OFFSET_NONE;
    this->_sConfigInjected_b.InjectedOffset = 0;
    this->_sConfigInjected_b.InjectedNbrOfConversion = 1;
    this->_sConfigInjected_b.InjectedDiscontinuousConvMode = DISABLE;
    this->_sConfigInjected_b.AutoInjectedConv = DISABLE;
    this->_sConfigInjected_b.QueueInjectedContext = DISABLE;
    // this->_sConfigInjected_b.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJEC_T1_TRGO2;
    this->_sConfigInjected_b.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
    // this->_sConfigInjected_b.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_RISING;
    this->_sConfigInjected_b.InjecOversamplingMode = DISABLE;
    // this->_sConfigInjected_b.InjecOversampling.Ratio = ADC_OVERSAMPLING_RATIO_2;
    // this->_sConfigInjected_b.InjecOversampling.RightBitShift = ADC_RIGHTBITSHIFT_3;

    // Phase C config
    // PA4, ADC2, Channel 17
    this->_sConfigInjected_c.InjectedChannel = PHASE_C_CHANNEL;
    this->_sConfigInjected_c.InjectedRank = ADC_REGULAR_RANK_1;
    this->_sConfigInjected_c.InjectedSamplingTime = ADC_SAMPLETIME_92CYCLES_5;
    this->_sConfigInjected_c.InjectedSingleDiff = ADC_SINGLE_ENDED;
    this->_sConfigInjected_c.InjectedOffsetNumber = ADC_OFFSET_NONE;
    this->_sConfigInjected_c.InjectedOffset = 0;
    this->_sConfigInjected_c.InjectedNbrOfConversion = 1;
    this->_sConfigInjected_c.InjectedDiscontinuousConvMode = DISABLE;
    this->_sConfigInjected_c.AutoInjectedConv = DISABLE;
    this->_sConfigInjected_c.QueueInjectedContext = DISABLE;
    // this->_sConfigInjected_c.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJEC_T1_TRGO;
    this->_sConfigInjected_c.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
    // this->_sConfigInjected_c.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_RISING;
    this->_sConfigInjected_c.InjecOversamplingMode = DISABLE;
    // this->_sConfigInjected_c.InjecOversampling.Ratio = ADC_OVERSAMPLING_RATIO_2;
    // this->_sConfigInjected_c.InjecOversampling.RightBitShift = ADC_RIGHTBITSHIFT_3;

    float volts_to_amps_ratio = 1.0f / shunt_resistor / gain; // volts to amps
    // gains for each phase
    this->gain_a = volts_to_amps_ratio;
    this->gain_b = volts_to_amps_ratio;
    this->gain_c = volts_to_amps_ratio;

    calibrateOffsets();

    // set the initialized flag
    this->initialized = true;
    // initialized = (params!=SIMPLEFOC_CURRENT_SENSE_INIT_FAILED);

    // return success
    return 1;
}
// Function finding zero offsets of the ADC
void LowsideCurrentSense::calibrateOffsets(){    
    const int calibration_rounds = 50;

    // find adc offset = zero current voltage
    this->offset_ia = 0;
    this->offset_ib = 0;
    this->offset_ic = 0;
    // read the adc voltage 1000 times ( arbitrary number )

    HAL_ADCEx_InjectedConfigChannel(this->_hadc_1, &(this->_sConfigInjected_a));
    for (int i = 0; i < calibration_rounds; i++) {
        HAL_ADCEx_InjectedStart(this->_hadc_1);
        HAL_ADCEx_InjectedPollForConversion(this->_hadc_1, HAL_MAX_DELAY);
        this->offset_ia += ((double)(HAL_ADCEx_InjectedGetValue(this->_hadc_1, ADC_REGULAR_RANK_1) >> 2) / (double)ADC_MAX) * VREF;
        HAL_ADCEx_InjectedStop(this->_hadc_1);
    }

    HAL_ADCEx_InjectedConfigChannel(this->_hadc_1, &(this->_sConfigInjected_b));
    for (int i = 0; i < calibration_rounds; i++) {
        HAL_ADCEx_InjectedStart(this->_hadc_1);
        HAL_ADCEx_InjectedPollForConversion(this->_hadc_1, HAL_MAX_DELAY);
        this->offset_ib += ((double)(HAL_ADCEx_InjectedGetValue(this->_hadc_1, ADC_REGULAR_RANK_1) >> 2) / (double)ADC_MAX) * VREF;
        HAL_ADCEx_InjectedStop(this->_hadc_1);
    }

    HAL_ADCEx_InjectedConfigChannel(this->_hadc_2, &(this->_sConfigInjected_c));
    for (int i = 0; i < calibration_rounds; i++) {
        HAL_ADCEx_InjectedStart(this->_hadc_2);
        HAL_ADCEx_InjectedPollForConversion(this->_hadc_2, HAL_MAX_DELAY);
        this->offset_ic += ((double)(HAL_ADCEx_InjectedGetValue(this->_hadc_2, ADC_REGULAR_RANK_1) >> 2) / (double)ADC_MAX) * VREF;
        HAL_ADCEx_InjectedStop(this->_hadc_2);
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

    HAL_ADCEx_InjectedConfigChannel(this->_hadc_1, &(this->_sConfigInjected_a));
    HAL_ADCEx_InjectedConfigChannel(this->_hadc_2, &(this->_sConfigInjected_c));
    HAL_ADCEx_InjectedStart(this->_hadc_1);
    HAL_ADCEx_InjectedStart(this->_hadc_2);
    HAL_ADCEx_InjectedPollForConversion(this->_hadc_1, HAL_MAX_DELAY);
    HAL_ADCEx_InjectedPollForConversion(this->_hadc_2, HAL_MAX_DELAY);
    current.a = ((((float)(HAL_ADCEx_InjectedGetValue(this->_hadc_1, ADC_REGULAR_RANK_1) >> 2) / (float)ADC_MAX) * VREF) - this->offset_ia) * this->gain_a;// amps
    current.c = ((((float)(HAL_ADCEx_InjectedGetValue(this->_hadc_2, ADC_REGULAR_RANK_1) >> 2) / (float)ADC_MAX) * VREF) - this->offset_ic) * this->gain_c; // amps
    HAL_ADCEx_InjectedStop(this->_hadc_1);
    HAL_ADCEx_InjectedStop(this->_hadc_2);

    current.a = lpf_a(current.a);
    current.c = lpf_c(current.c);

    // HAL_ADC_ConfigChannel(this->_hadc_1, &(this->_sConfig_b));
    // HAL_ADC_Start(this->_hadc_1);
    // HAL_ADC_PollForConversion(this->_hadc_1, HAL_MAX_DELAY);
    // current.b = ((((float)(HAL_ADC_GetValue(this->_hadc_1)) / (float)ADC_MAX) * VREF) - this->offset_ib) * this->gain_b;// amps
    // HAL_ADC_Stop(this->_hadc_1);
    current.b = -1 * (current.a + current.c);

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
