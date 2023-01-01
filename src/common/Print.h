#ifndef PRINT_H
#define PRINT_H

#include <string>

class Print {
public:
	Print();
	void print(std::string to_print);
    void print(float value, int format);
    void println();
};

#endif