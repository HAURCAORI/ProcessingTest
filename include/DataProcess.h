#pragma once
#include "Struct.h"
#include <iostream>
#include <vector>
#include <time.h>
#include "Calculate.h"

using namespace std;

//함수 선언부 (IO)
void CreateEmptyFile(const PAGE page);
bool LoadFile(const PAGE page);
bool UnloadFile(const PAGE page);
bool isLoaded(const PAGE page);
PageFile* getPage(const PAGE page);
void SpecificDataRead(PAGE page, SECTOR sector);
bool InsertDataHeader(PAGE page, SECTOR sector, BYTE type);
bool InsertAddress(PAGE page, SECTOR sector, int offset, BYTES value);
bool ClearData(PAGE page, SECTOR sector);
NUMBER UpDownData(FILE *stream, bool increase);

bool isAvailableAddress(FILE *stream);
BYTE TypeGen(bool isValide, bool isTerminus);
BYTE TypeDefault();
Signal SignalGen(float value);
BYTE PropertyGen();
BYTE FlagGen();

//함수 처리부 (Process)
bool Load(PAGE page, SECTOR sector, Signal signal);
bool UnloadProcess();
void ShowProcess();
bool UnloadNeuron(Neuron *neuron);

void test();