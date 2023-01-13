#include "Sensor.h"
#include "../foc_utils.h"
#include "../time_utils.h"
#include <cmath>

#define VEL_CALC_HIST_LEN 50
#define STD_DEVIATIONS 0.1

void Sensor::update() {
    uint32_t before_timestamp = _micros();
    float val = getSensorAngle();
    angle_prev_ts = ((_micros() - before_timestamp) / 2) + before_timestamp;
    float d_angle = val - angle_prev;
    // if overflow happened track it as full rotation
    if(abs(d_angle) > (0.8f*_2PI) ) full_rotations += ( d_angle > 0 ) ? -1 : 1;
    angle_prev = val;


    const float threshold = _PI;

    if (angle_prev < threshold && top_bottom == 1) {
        top_bottom = 0;

        // Linear interpolation of new timestamp for crossing threshold
        xts_1 = xts_2;
        xts_2 = xts_3;
        xts_3 = (((angle_prev_ts - vel_angle_prev_ts) / (angle_prev - vel_angle_prev)) * (threshold - vel_angle_prev)) + vel_angle_prev_ts;

        // Calculate the new velocity
        if (xts_3 != xts_1)
            velocity = 1 / ((xts_3 - xts_1) * 1e-6);
    } else if (angle_prev > threshold && top_bottom == 0) {
        top_bottom = 1;

        // Linear interpolation of new timestamp for crossing threshold
        xts_1 = xts_2;
        xts_2 = xts_3;
        xts_3 = (((angle_prev_ts - vel_angle_prev_ts) / (angle_prev - vel_angle_prev)) * (threshold - vel_angle_prev)) + vel_angle_prev_ts;

        // Calculate the new velocity
        if (xts_3 != xts_1)
            velocity = 1 / ((xts_3 - xts_1) * 1e-6);
    }

    vel_angle_prev = angle_prev;
    vel_angle_prev_ts = angle_prev_ts;
}


 /** get current angular velocity (rad/s) */
float Sensor::getVelocity() {
    // calculate sample time
    // float Ts = (angle_prev_ts - vel_angle_prev_ts)*1e-6;
    // if (Ts < min_elapsed_time) return velocity; // don't update velocity if Ts is too small

    // velocity = ( (float)(full_rotations - vel_full_rotations)*_2PI + (angle_prev - vel_angle_prev) ) / Ts;
    // float angle_diff = atan2(sin(angle_prev - vel_angle_prev), cos(angle_prev - vel_angle_prev));
    // velocity = angle_diff / Ts;
    // velocity = (angle_prev - vel_angle_prev) / Ts;

    
    return velocity;
    

    // vel_full_rotations = full_rotations;
    // return velocity;

    // Persistent variables
    // static float velocity_history[VEL_CALC_HIST_LEN];
    // static int history_index = 0;
    // static int history_ready = 0;
    // static float previous_adjusted_mean = 0;

    // Non-persisting variables
    // float unadjusted_mean = 0;
    // float std_dev_summation = 0;
    // float std_dev;
    // float lower_bound;
    // float upper_bound;
    // int num_normal_vels = 0;
    // float adjusted_mean = 0;

    // if (history_index == VEL_CALC_HIST_LEN) {
    //     history_index = 0;
    //     history_ready = 1;
    // }

    // velocity_history[history_index] = velocity;
    // history_index++;

    // if (history_ready) {
    //     // Calculate an "unadjusted" mean
    //     for (int i = 0; i < VEL_CALC_HIST_LEN; i++)
    //         unadjusted_mean += velocity_history[i];
    //     unadjusted_mean /= VEL_CALC_HIST_LEN;

    //     // Calculate std dev
    //     for (int i = 0; i < VEL_CALC_HIST_LEN; i++)
    //         std_dev_summation += pow(velocity_history[i] - unadjusted_mean, 2);
    //     std_dev = sqrt(std_dev_summation / VEL_CALC_HIST_LEN);

    //     // Find bounds for data
    //     lower_bound = unadjusted_mean - (std_dev * STD_DEVIATIONS);
    //     upper_bound = unadjusted_mean + (std_dev * STD_DEVIATIONS);

    //     if (velocity <= upper_bound && velocity >= lower_bound) {
    //         previous_adjusted_mean = velocity;
    //         return velocity;
    //     } else return previous_adjusted_mean;

        // Calculate adjusted mean
        // for (int i = 0; i < VEL_CALC_HIST_LEN; i++)
        //     if (velocity_history[i] <= upper_bound && velocity_history[i] >= lower_bound) {
        //        adjusted_mean += velocity_history[i];
        //        num_normal_vels++;
        //     }
        // if (num_normal_vels) {
        //     adjusted_mean /= num_normal_vels;
        //     previous_adjusted_mean = adjusted_mean;
        //     return adjusted_mean;
        // } else
        //     return previous_adjusted_mean;
    // } else
    //     return velocity;
}



// void Sensor::init() {
//     // initialize all the internal variables of Sensor to ensure a "smooth" startup (without a 'jump' from zero)
//     getSensorAngle(); // call once
//     _delay(1);
//     vel_angle_prev = getSensorAngle(); // call again
//     vel_angle_prev_ts = _micros();
//     _delay(1);
//     getSensorAngle(); // call once
//     _delay(1);
//     angle_prev = getSensorAngle(); // call again
//     angle_prev_ts = _micros();
// }


float Sensor::getMechanicalAngle() {
    return angle_prev;
}



float Sensor::getAngle(){
    return (float)full_rotations * _2PI + angle_prev;
}



double Sensor::getPreciseAngle() {
    return (double)full_rotations * (double)_2PI + (double)angle_prev;
}



int32_t Sensor::getFullRotations() {
    return full_rotations;
}



int Sensor::needsSearch() {
    return 0; // default false
}
