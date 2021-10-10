#pragma once
#include "Struct.h"
#include <iostream>
#include <vector>
#include <time.h>
#include "Calculate.h"

using namespace std;

//함수 선언부 (IO)
bool Initialize();
void CreateEmptyFile(const PAGE page);
bool DeleteFile(const PAGE page);
void DeleteAllFile();
bool LoadFile(const PAGE page);
bool UnloadFile(const PAGE page);
bool isLoaded(const PAGE page);
PageFile* getPage(const PAGE page);
void SpecificDataRead(PAGE page, SECTOR sector);
bool InsertDataHeader(PAGE page, SECTOR sector, BYTE type, NUMBER count);
bool InsertDataHeader(PAGE page, SECTOR sector, BYTE type, NUMBER count, float threshold, float weight);
bool InsertAddress(PAGE page, SECTOR sector, int offset, BYTES value);
bool InsertAddressAuto(Neuron* neuron, BYTES value);
bool ClearData(PAGE page, SECTOR sector);
void SetZero(FILE* &stream, long pos);
NUMBER UpDownData(FILE* &stream, long pos, bool increase);

bool isAvailableAddress(FILE *stream, long pos);
BYTE TypeGen(bool isValide, bool isTerminus);
BYTE TypeDefault();
Signal SignalGen(float value);
BYTE PropertyGen();
BYTE FlagGen();

//함수 처리부 (Process)
bool Load(PAGE page, SECTOR sector, Signal* signal, Neuron* previous);
bool UnloadProcess();
void ShowProcess();
bool UnloadNeuron(Neuron *neuron);

//Thread
void ffread(FILE* &stream, long& pos, BYTE& sender);
void ffread(FILE* &stream, long& pos, BYTES& sender);
void ffread(FILE* &stream, long& pos, float& sender);
void ffwrite(FILE* &stream, long& pos, BYTE& sender);
void ffwrite(FILE* &stream, long& pos, BYTES& sender);
void ffwrite(FILE* &stream, long& pos, float& sender);
void CycleThread();