#include "DataProcess.h"
#include <fstream>

vector<PageFile> pfile;

void CreateEmptyFile(PAGE page)
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

void LoadFile(PAGE page)
{
    
}

bool isLoaded(PAGE page)
{
    
}