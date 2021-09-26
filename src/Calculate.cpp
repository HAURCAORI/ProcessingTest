#include "Calculate.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <sstream>

#define RANGE 0.5

// random device class instance, source of 'true' randomness for initializing random seed
std::random_device rd; 
// Mersenne twister PRNG, initialized with seed from previous random device instance
std::mt19937 gen(rd()); 

float threshold_mean = 0.2;
float threshold_stddev = 0.1;
float weight_mean = 1.0;
float weight_stddev = 0.1;


float random_float()
{
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

float random_threshold()
{
    std::normal_distribution<float> dist(threshold_mean, threshold_stddev);
    return dist(gen);
}

float random_weight()
{
    std::normal_distribution<float> dist(weight_mean, weight_stddev);
    return dist(gen);
}

bool isnumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

vector<string> split(string str, char delimiter)
{
	vector<string> vs;
	string temp;

	for (int i = 0; !(str[i] == 13 /* Line Feed */ || str[i] == '\n' || str[i] == 0); i++)
	{
		if (str[i] == delimiter)
		{
			vs.push_back(temp);
			temp.clear();

			continue;
		}

		temp.push_back(str[i]);
	}

	vs.push_back(temp);

	return vs;
}
