#include "Calculate.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <sstream>
#include <vector>

#define RANGE 0.5

float random_float() {
    std::srand(std::time(nullptr));
    return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/RANGE));
}

unsigned char* getByteString(const BYTE byte)
{
    BYTE b = byte;
	static unsigned char result[8];
	for (int i = 7; i >= 0; --i) {
		result[i] = '0' + (b & 1);
        b >>= 1;
	}
	return result;
}

BIT getBit(const BYTE byte, const int n)
{
    return (byte >> (7 - n)) & 1;
}