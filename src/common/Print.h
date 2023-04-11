#ifndef PRINT_H
#define PRINT_H

#include <string>

class Print {
public:
	Print(int(*write_wrapper)(const char *, size_t));
	void print(std::string to_print);
    void print(float value, int format);
    void println();
private:
    int (*write)(const char *, size_t);
};

#endif