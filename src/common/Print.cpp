#include "Print.h"
#include <cmath>
#include <cfloat>

Print::Print(int(*write_wrapper)(const char *, size_t)) {
    write = write_wrapper;
}

void Print::print(std::string to_print) {
    write(to_print.c_str(), to_print.length());
}

void Print::print(float value, int format) {
    // THIS CODE TAKEN FROM ARDUINO SOURCE CODE
    // size_t n = 0;
  
    // if (isnan(value)) return print("nan");
    if (std::isnan(value)) return;
    // if (isinf(value)) return print("inf");
    if (std::isinf(value)) return;
    // if (value > 4294967040.0) return print ("ovf");  // constant determined empirically
    // if (value <-4294967040.0) return print ("ovf");  // constant determined empirically
    
    // Handle negative numbers
    if (value < 0.0)
    {
        // n += this->print("-");
        this->print("-");
        value = -value;
    }

    // Round correctly so that print(1.999, 2) prints as "2.00"
    float rounding = 0.5;
    for (uint8_t i=0; i<format; ++i)
        rounding /= 10.0;
    
    value += rounding;

    // Extract the integer part of the number and print it
    unsigned long int_part = (unsigned long)value;
    float remainder = value - (double)int_part;
    // n += print(int_part);
    this->print(std::to_string(int_part));

    // Print the decimal point, but only if there are digits beyond
    if (format > 0) {
        // n += print('.');
        this->print(".");
    }

    // Extract digits from the remainder one at a time
    while (format-- > 0)
    {
        remainder *= 10.0;
        unsigned int toPrint = (unsigned int)(remainder);
        // n += print(toPrint);
        this->print(std::to_string(toPrint));
        remainder -= toPrint;
    } 
    
    // return n;
}

void Print::println() {
    std::string newline = "\r\n";
    write(newline.c_str(), newline.length());
}