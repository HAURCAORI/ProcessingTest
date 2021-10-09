#include "DataProcess.h"
#include <fstream>

vector<PageFile> pfile;
PageFile pin;
PageFile pout;

bool Initialize()
{
	string address = (string)Path + "INPUT";
	FILE* streamin = fopen(address.c_str(), "r+");
	if(streamin) {
		return false;
	}
	pin.stream = streamin;
	pin.page = USHORT_INPUT;

	address = (string)Path + "OUTPUT";
	FILE* streamout = fopen(address.c_str(), "r+");
	if(streamin) {
		return false;
	}
	pout.stream = streamout;
	pout.page = USHORT_OUTPUT;

	return true;
}

void CreateEmptyFile(const PAGE page)
{
	vector<char> empty(1024, 0);
	if(page == USHORT_INPUT)
	{
		string address = (string) Path + "INPUT";
		ofstream ofs(address, ios::binary | ios::out);

		for (int i = 0; i < SectorSize; i++)
		{
			if (!ofs.write(&empty[0], empty.size()))
			{
				cout << "problem writing to file" << endl;
			}
		}
	}else if (page == USHORT_OUTPUT)
	{
		string address = (string) Path + "OUTPUT";
		ofstream ofs(address, ios::binary | ios::out);

		for (int i = 0; i < SectorSize; i++)
		{
			if (!ofs.write(&empty[0], empty.size()))
			{
				cout << "problem writing to file" << endl;
			}
		}
	}else
	{
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
}

bool DeleteFile(const PAGE page)
{
	string address = (string) Path + to_string(page);
	if(remove(address.c_str())!=0)
	{
		return false;
	}
	else
	{
		return true;
	}
	
}

void DeleteAllFile()
{
	for(int i = 0; i <= USHORT_MAX; i++)
	{
		string address = (string) Path + to_string(i);
		remove(address.c_str());
	}
}

bool LoadFile(const PAGE page)
{
	if(!isLoaded(page))
	{	
		string address = (string)Path + to_string(page);
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
	if (page == USHORT_INPUT)
	{
		return &pin;
	}
	else if (page == USHORT_OUTPUT)
	{
		return &pout;
	}
	else
	{
		for (vector<PageFile>::size_type i = 0; i < pfile.size(); i++)
		{
			if (pfile[i].page == page)
			{
				return &pfile[i];
			}
		}
		if (LoadFile(page))
			return &pfile[pfile.size() - 1];
		else
			return NULL;
	}
}

void SpecificDataRead(PAGE page, SECTOR sector)
{
	string address = (string)Path + to_string(page);
	FILE *stream = fopen(address.c_str(), "r+");
	if(stream) {
		long pos = SectorUnit * sector;
		BYTE byte;
		for(int i = 0; i < SectorUnit; i ++)
		{
			ffread(stream, pos, byte);
			++pos;
			cout << getByteString(byte) << "/";
		}
		fclose(stream);
	}
}

bool InsertDataHeader(PAGE page, SECTOR sector, BYTE type, NUMBER count)
{
	NUMBER empty = 0;
	string address;
	if (page == USHORT_INPUT)
		address = (string) Path + "INPUT";
	else if (page == USHORT_OUTPUT)
		address = (string) Path + "OUTPUT";
	else
		address = (string) Path + to_string(page);
		
	FILE *stream = fopen(address.c_str(), "r+");
	if(stream) {
		long pos = SectorUnit * sector;
		ffwrite(stream,pos,type);
		++pos;
		ffwrite(stream,pos,count);
		++pos;
		ffwrite(stream,pos,empty); //priority
		++pos;
		ffwrite(stream,pos,empty); //extra
		++pos;
		float threshold = random_threshold();
		ffwrite(stream, pos, threshold);
		pos += 4;
		float weight = random_weight();
		ffwrite(stream, pos, weight);
		pos += 4;
		float temp = 0;
		ffwrite(stream, pos, temp);
		fclose(stream);
		return true;
	}else{
		return false;
	}
	
}

bool InsertDataHeader(PAGE page, SECTOR sector, BYTE type,  NUMBER count, float threshold, float weight)
{
	NUMBER empty = 0;
	string address = (string) Path + to_string(page);
	FILE *stream = fopen(address.c_str(), "r+");
	if(stream) {
		long pos = SectorUnit * sector;
		ffwrite(stream,pos,type);
		++pos;
		ffwrite(stream,pos,count);
		++pos;
		ffwrite(stream,pos,empty); //priority
		++pos;
		ffwrite(stream,pos,empty); //extra
		++pos;
		ffwrite(stream, pos, threshold);
		pos += 4;
		ffwrite(stream, pos, weight);
		pos += 4;
		float temp = 0;
		ffwrite(stream, pos, temp);
		fclose(stream);
		return true;
	}else{
		return false;
	}
}

bool InsertAddress(PAGE page, SECTOR sector, int offset, BYTES value)
{
	string address = (string)Path + to_string(page);
	FILE *stream = fopen(address.c_str(), "r+");
	if(stream) {
		long pos = SectorUnit * sector + offset;
		if(isAvailableAddress(stream, pos))
		{
			ffwrite(stream, pos, value);
			fclose(stream);
			return true;
		}else{
			return false;
		}
	}else{
		return false;
	}
}

bool InsertAddressAuto(Neuron* neuron, BYTES value)
{
	long pos = SectorUnit*(neuron->sector) + 16 + (neuron->count)*2;

	if(isAvailableAddress(neuron->stream, pos))
	{
		ffwrite(neuron->stream, pos, value);
		pos += 2;
		BYTES extra = USHORT_MAX;
		ffwrite(neuron->stream, pos, extra);
		pos = SectorUnit*(neuron->sector) + 1;
		UpDownData(neuron->stream, pos, true);
		return true;
	}else{
		return false; //주소 데이터 삽입 불가시 처리구문 추가
	}
}

bool ClearData(PAGE page, SECTOR sector)
{
	string address = (string)Path + to_string(page);
	FILE *stream = fopen(address.c_str(), "r+");
	float empty = 0;
	if(stream) {
		long pos = SectorUnit*sector;
		ffwrite(stream, pos, empty);
		pos += 4;
		ffwrite(stream, pos, empty);
		pos += 4;
		ffwrite(stream, pos, empty);
		pos += 4;
		ffwrite(stream, pos, empty);
		fclose(stream);
		return true;
	}else{
		return false;
	}
}

bool isAvailableAddress(FILE *stream, long pos)
{
	BYTES previous;
	ffread(stream, pos, previous);
	pos += 2;
	if(previous == 0 || previous == USHORT_MAX)
	{
		ffread(stream, pos ,previous);
		if(previous == 0 || previous == USHORT_MAX)	
			return true;
		else
			return false;		
	}
	else
	{
		return false;
	}
}

NUMBER UpDownData(FILE *stream, long pos, bool increase)
{
	NUMBER count;
	if(increase)
	{
		ffread(stream, pos, count);
		count++;
		ffwrite(stream, pos, count);
	}
	else
	{
		ffread(stream, pos, count);
		count--;
		ffwrite(stream, pos, count);
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

BYTE FlagGen()
{
	return 0;
}