#pragma once
#include <stdio.h>
#define SectorCount 65536
#define SectorSize 1024 //KB
#define SectorUnit 16 //BYTE
// (SectorSize) = (SectorCount)*(SectorUnit)
#define ReadingUnit 4
#define NeuronHeaderSize 4 //BYTE
#define NeuronValueSize 12 //BYTE
#define Path "resource/"

typedef unsigned char BYTE;
typedef unsigned char BIT;
typedef unsigned char NUMBER;
typedef unsigned short COUNT;
typedef float TIMESTAMP;

typedef unsigned short PAGE;
typedef unsigned short SECTOR;
typedef char ADDRESS;

struct Signal
{
    BYTE type;
    float value;
    COUNT count;
};

struct Neuron
{
    PAGE page;
    SECTOR sector;
    //Header
    BYTE type;
    NUMBER priority;
    NUMBER size; //except Header
    BYTE extra;
    //Value
    float threshold;
    float weight;
    //Address
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
    TIMESTAMP timestamp;
};