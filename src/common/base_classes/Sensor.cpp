#include "Sensor.h"
#include "../foc_utils.h"
#include "../time_utils.h"



void Sensor::update() {
    float val = getSensorAngle();
    angle_prev_ts = _micros();
    float d_angle = val - angle_prev;
    // if overflow happened track it as full rotation
    if(abs(d_angle) > (0.8f*_2PI) ) full_rotations += ( d_angle > 0 ) ? -1 : 1; 
    angle_prev = val;
}


 /** get current angular velocity (rad/s) */
float Sensor::getVelocity() {
    // calculate sample time
    float Ts = (angle_prev_ts - vel_angle_prev_ts)*1e-6;
    if (Ts < min_elapsed_time) return velocity; // don't update velocity if Ts is too small

    // The angle shouldnt be huge -- check for step from 0 back up to 2pi
    if ((angle_prev - vel_angle_prev) < (_2PI / 6.0f)) {
        // velocity = (angle_prev - vel_angle_prev) / Ts;
        velocity = ( (float)(full_rotations - vel_full_rotations)*_2PI + (angle_prev - vel_angle_prev) ) / Ts;
    }
    
    velocity_history[velocity_history_index] = velocity;
    float avg_velocity = 0;
    for (int i = 0; i < VELOCITY_HISTORY_LEN; ++i) {
        avg_velocity += velocity_history[i];
    }
    avg_velocity /= VELOCITY_HISTORY_LEN;

    ++velocity_history_index;
    if (velocity_history_index == VELOCITY_HISTORY_LEN)
        velocity_history_index = 0;

    vel_angle_prev = angle_prev;
    vel_full_rotations = full_rotations;
    vel_angle_prev_ts = angle_prev_ts;
    return avg_velocity;
}



void Sensor::init() {
    // // initialize all the internal variables of Sensor to ensure a "smooth" startup (without a 'jump' from zero)
    // getSensorAngle(); // call once
    // delayMicroseconds(1);
    // vel_angle_prev = getSensorAngle(); // call again
    // vel_angle_prev_ts = _micros();
    // delay(1);
    // getSensorAngle(); // call once
    // delayMicroseconds(1);
    // angle_prev = getSensorAngle(); // call again
    // angle_prev_ts = _micros();
}


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