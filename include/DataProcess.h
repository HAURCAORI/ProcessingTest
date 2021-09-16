#pragma once
#include "Struct.h"
#include <iostream>
#include <vector>
#include <time.h>

using namespace std;

//함수 선언부
void CreateEmptyFile(const PAGE page);
bool LoadFile(const PAGE page);
bool UnloadFile(const PAGE page);
bool isLoaded(const PAGE page);
PageFile* getPage(const PAGE page);