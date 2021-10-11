#pragma once
#include <stdio.h>
#define SectorCount 65536
#define SectorSize 2048 //KB 1024
#define SectorUnit 32 //BYTE 16
// (SectorSize) = (SectorCount)*(SectorUnit)
#define ReadingUnit 4
#define NeuronHeader 18
#define InputHeader 8
//#define NeuronHeaderSize 4 //BYTE
//#define NeuronValueSize 12 //BYTE
#define Path "resource/"
#define USHORT_MAX 65535
#define USHORT_TRA 65534 //SECTOR의 경우
#define USHORT_INPUT 65535 //PAGE의 경우
#define USHORT_OUTPUT 65534 //PAGE의 경우

#define Pos_Priority 4
#define Pos_Temp 14

typedef unsigned char BYTE;
typedef unsigned char BIT;
typedef unsigned char NUMBER;
typedef unsigned char FLAG;
typedef unsigned short COUNT;
typedef float TIMESTAMP;

typedef unsigned short PAGE;
typedef unsigned short SECTOR;
typedef unsigned short BYTES;
//typedef char ADDRESS;

struct Signal
{
    BYTE specificity;
    float value;
    COUNT count;
};

struct Neuron
{
    FILE* stream; //빠르게 접근하기 위한 stream
    Neuron* previous = nullptr;
    PAGE page;
    SECTOR sector;
    //Header
    BYTE type;
    // 1 bit : 활성화 유무 | 2 bit : 수정 불가(fix) | 3 bit : 고정 value 출력 | 4 bit : 
    // 5 bit : | 6 bit : | 7 bit : branch여부 | 8 bit : 주소가 없는 Neuron
    // 추가해야 할 것 = 
    // 2 bit : true일 경우 데이터 변경 불가
    // 3 bit : 
    // 4 bit :
    // 5 bit : 
    // 6 bit : 
    // 7 bit : branch가 true일 경우 previous 무시
    // 8 bit : 다음 주소로 무조건 전달함 주소 변경 불가
    NUMBER count;// equals to address count(except Header) [per 2bytes]
    FLAG specificity; // 수용기에 대한 정보
    BYTE extra; // temp
    NUMBER priority; //통계를 위한 값
    NUMBER effective; //통계를 위한 값 불필요한 경우 향후 struct에서 삭제
    //Value
    float threshold;
    float weight;
    float temp;
    //ActiveNeuronSet

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