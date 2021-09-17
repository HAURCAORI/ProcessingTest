#include "DataProcess.h"

bool Load(PAGE page, SECTOR sector, Signal signal)
{
    FILE *stream = getPage(page)->stream;
    Neuron neuron;
    neuron.page = page;
    neuron.sector = sector;

    fseek(stream, SectorUnit * sector, SEEK_SET);

    fread(&neuron.type, sizeof(BYTE), 1, stream);
	fread(&neuron.priority, sizeof(NUMBER), 1, stream);
    fread(&neuron.size, sizeof(NUMBER), 1, stream);
    fread(&neuron.extra, sizeof(BYTE), 1, stream);
}