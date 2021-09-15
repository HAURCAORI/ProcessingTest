#include "Calculate.h"
#include <iostream>
#include <iomanip>
#include <random>

#define RANGE 0.5

float random_float() {
    std::srand(std::time(nullptr));
    return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/RANGE));
}