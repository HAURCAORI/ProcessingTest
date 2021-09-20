#include "DataProcess.h"
#include <fstream>

vector<PageFile> pfile;

void CreateEmptyFile(const PAGE page)
{
    vector<char> empty(1024, 0);
	string address = (string) Path + to_string(page);
	ofstream ofs(address, ios::binary | ios::out);

	for (int i = 0; i < SectorSize; i++)
	{
		if (!ofs.write(&empty[0], empty.size()))
		{
			cout << "problem writing to file" << endl;
		}
	}
}

bool LoadFile(const PAGE page)
{
	if(!isLoaded(page))
	{	
		string address = (string)Path + "0";
		FILE *stream = fopen(address.c_str(), "r+");
		if(!stream) {
			return false;
		}
		PageFile pf = {stream,page, (float) clock()};
		pfile.push_back(pf);
		return true;
	}
	else
		return true;
}

bool UnloadFile(const PAGE page)
{
	for (vector<PageFile>::size_type i = 0; i < pfile.size();) {
        if (pfile[i].page == page)
		{
			fclose(pfile[i].stream);
			pfile.erase(pfile.begin() + i);
            return true;
		}else{
			i++;
		}
    }
	return false;
}

bool isLoaded(const PAGE page)
{
    for (vector<PageFile>::size_type i = 0; i < pfile.size();i++) {
        if (pfile[i].page == page)
		{
            return true;
		}
    }
	return false;
}

PageFile* getPage(const PAGE page)
{
	for (vector<PageFile>::size_type i = 0; i < pfile.size();i++) {
        if (pfile[i].page == page)
		{
            return &pfile[i];
		}
    }
	if(LoadFile(page))
		return &pfile[pfile.size()-1];
	else
		return NULL;
}

void SpecificDataRead(PAGE page, SECTOR sector)
{
	string address = (string)Path + "0";
	FILE *stream = fopen(address.c_str(), "r+");
	if(stream) {
		fseek(stream, SectorUnit*sector, SEEK_SET);
		BYTE byte;
		for(int i = 0; i < SectorUnit; i ++)
		{
			fread(&byte, sizeof(BYTE), 1, stream);
			cout << getByteString(byte) << "/";
		}
		fclose(stream);
	}
}

bool InsertDataHeader(PAGE page, SECTOR sector, BYTE type)
{
	NUMBER empty = 0;
	NUMBER count = 0;

	string address = (string)Path + "0";
	FILE *stream = fopen(address.c_str(), "r+");
	if(stream) {
		fseek(stream, SectorUnit*sector, SEEK_SET);
		fwrite(&type, 1, 1, stream); //type
		fwrite(&count, 1, 1, stream);//size
		fwrite(&empty, 1, 1, stream);//priority
		fwrite(&empty, 1, 1, stream);//extra
		float threshold = random_threshold();
		fwrite(&threshold, sizeof(float), 1, stream);
		float weight = random_weight();
		fwrite(&weight, sizeof(float), 1, stream);
		float temp = 0;
		fwrite(&temp, sizeof(float), 1, stream);
		fclose(stream);
		return true;
	}else{
		return false;
	}
}

bool InsertAddress(PAGE page, SECTOR sector, int offset, BYTES value)
{
	string address = (string)Path + "0";
	FILE *stream = fopen(address.c_str(), "r+");
	if(stream) {
		fseek(stream, SectorUnit*sector + offset, SEEK_SET);

		if(isAvailableAddress(stream))
		{
			fwrite(&value, sizeof(BYTES), 1, stream);
			fclose(stream);
			return true;
		}else{
			return false;
		}
	}else{
		return false;
	}
}

bool InsertAddressAuto(Neuron* neuron, FILE* stream, BYTES value)
{
	fseek(stream, SectorUnit*(neuron->sector) + 16 + (neuron->count)*2, SEEK_SET);

	if(isAvailableAddress(stream))
	{
		fwrite(&value, sizeof(BYTES), 1, stream);
		BYTES extra = USHORT_MAX;
		fwrite(&extra, sizeof(BYTES), 1, stream);
		++neuron->count;
	}else{
		return false; //주소 데이터 삽입 불가시 처리구문 추가
	}
}

bool ClearData(PAGE page, SECTOR sector)
{
	string address = (string)Path + "0";
	FILE *stream = fopen(address.c_str(), "r+");
	int empty = 0;
	if(stream) {
		fseek(stream, SectorUnit*sector, SEEK_SET);
		fwrite(&empty, 4, 1, stream);
		fwrite(&empty, 4, 1, stream);
		fwrite(&empty, 4, 1, stream);
		fwrite(&empty, 4, 1, stream);
		fclose(stream);
		return true;
	}else{
		return false;
	}
}

bool isAvailableAddress(FILE *stream)
{
	BYTES previous;
	fread(&previous, sizeof(BYTES), 1, stream);
	if(previous == 0 || previous == USHORT_MAX)
	{
		fread(&previous, sizeof(BYTES), 1, stream);
		fseek(stream, -4L, SEEK_CUR);
		if(previous == 0 || previous == USHORT_MAX)	
			return true;
		else
			return false;		
	}
	else
	{
		fseek(stream, -2L, SEEK_CUR);
		return false;
	}
}

NUMBER UpDownData(FILE *stream, bool increase)
{
	NUMBER count;
	if(increase)
	{
		fread(&count, sizeof(NUMBER), 1, stream);
		++count;
		fseek(stream, -1L, SEEK_CUR);
		fwrite(&count, sizeof(NUMBER), 1, stream);
	}
	else
	{
		fread(&count, sizeof(NUMBER), 1, stream);
		--count;
		fseek(stream, -1L, SEEK_CUR);
		fwrite(&count, sizeof(NUMBER), 1, stream);
	}
	return count;
}

BYTE TypeGen(bool isValide, bool isTerminus)
{
	BYTE byte;
	byte = isValide; //데이터에 Neuron이 할당되면 true값으로
	byte <<= 1;
	byte |= isTerminus; //true일 경우 address자리를 명령으로 처리
	byte <<= 6;
	return byte;
}

BYTE TypeDefault()
{
	BYTE byte;
	byte = 1;
	byte <<= 1;
	byte |= 0;
	byte <<= 6;
	return byte;
}

Signal SignalGen(float value)
{
	Signal signal = {PropertyGen(),value,0};
	return signal;
}

BYTE PropertyGen()
{
	BYTE byte = 0;
	return byte;
}