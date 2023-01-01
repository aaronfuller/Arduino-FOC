#include "time_utils.h"

// function buffering delay() 
// arduino uno function doesn't work well with interrupts
void _delay(unsigned long ms){
  HAL_Delay(ms);
}


// function buffering _micros() 
// arduino function doesn't work well with interrupts
unsigned long _micros(){
  return TIM2->CNT;
}
