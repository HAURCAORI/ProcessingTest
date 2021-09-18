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

BYTE TypeGen(bool isValide, bool isTerminus);
BYTE TypeDefault();
BYTE PropertyGen();