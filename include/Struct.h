#pragma once
#include <stdio.h>
#define SectorCount 65536

typedef unsigned char BYTE;
typedef unsigned char NUMBER;
typedef unsigned short COUNT;
typedef float TIMESTAMP;

typedef unsigned short PAGE;
typedef unsigned short SECTOR;
typedef char ADDRESS;

struct Signal
{
    BYTE header;
    float value;
    COUNT count;
};

struct Neuron
{
    PAGE page;
    SECTOR sector;
    BYTE header;
    NUMBER priority;
    NUMBER size;
    BYTE extra;
    float threshold;
    float weight;
    ADDRESS address[0]; //flexible array member
};

struct ActiveNeuron
{
    struct Neuron neuron;
    TIMESTAMP timestamp;
    COUNT count;
    BYTE flag;
    float value;
};

struct PageFile
{
    FILE *stream;
    PAGE page;
};