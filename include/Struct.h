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
#define USHORT_MAX 65535

typedef unsigned char BYTE;
typedef unsigned char BIT;
typedef unsigned char NUMBER;
typedef unsigned short COUNT;
typedef float TIMESTAMP;

typedef unsigned short PAGE;
typedef unsigned short SECTOR;
typedef unsigned short BYTES;
//typedef char ADDRESS;

struct Signal
{
    BYTE property;
    float value;
    COUNT count;
};

struct Neuron
{
    FILE* stream; //빠르게 접근하기 위한 stream
    PAGE page;
    SECTOR sector;
    //Header
    BYTE type;
    // 1 bit : 활성화 유무 | 2 bit : isTerminus | 3 bit : 값 변경 유무(threshold, weight) | 4 bit : isChanged(주소 변경 유무)
    // 5 bit : 주소가 다 찼을 경우 fix 여부 | 6 bit : | 7 bit : | 8 bit 
    // 추가해야 할 것 = 
    // 3 bit와 4 bit는 수정 즉시 stream에 저장
    NUMBER count;// equals to address count(except Header) [per 2bytes]
    NUMBER priority; //통계를 위한 값
    NUMBER extra; //통계를 위한 값 불필요한 경우 향후 struct에서 삭제
    //Value
    float threshold;
    float weight;
    float temp;
    //Address
    BYTES address[0]; //flexible array member
};

struct ActiveNeuron
{
    struct Neuron* neuron;
    TIMESTAMP timestamp;
    BYTE flag;
};

struct PageFile
{
    FILE *stream;
    PAGE page;
    TIMESTAMP timestamp;
};