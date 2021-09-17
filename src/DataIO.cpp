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
		FILE *stream = fopen(address.c_str(), "r");
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
		return &pfile[pfile.size()];
	else
		return NULL;
}

void SpecificDataRead(PAGE page, SECTOR sector)
{
	string address = (string)Path + "0";
	FILE *stream = fopen(address.c_str(), "r");
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