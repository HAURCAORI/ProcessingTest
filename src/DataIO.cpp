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

void LoadFile(const PAGE page)
{
	string address = (string)Path + "0";
	FILE *stream = fopen(address.c_str(), "r");
	if(!stream) {
		return;
    }
	PageFile pf = {stream,page, clock()};
	pfile.push_back(pf);
}

bool UnloadFile(const PAGE page)
{
	for (vector<PageFile>::size_type i = 0; i < pfile.size();) {
        if (pfile[i].page == page)
		{
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
    for (vector<PageFile>::size_type i = 0; i < pfile.size();) {
        if (pfile[i].page == page)
		{
            return true;
		}
    }
	return false;
}