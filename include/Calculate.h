#pragma once
#include "Struct.h"
#include <vector>
#include <string>

using namespace std;

float random_float();
unsigned char* getByteString(const BYTE byte);
BIT getBit(const BYTE byte, const int n);

float random_threshold();
float random_weight();

vector<string> split(string str, char delimiter);