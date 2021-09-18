#include "DataProcess.h"

bool Load(PAGE page, SECTOR sector, Signal signal)
{
    FILE *stream = getPage(page)->stream;
    Neuron neuron;
    neuron.page = page;
    neuron.sector = sector;

    fseek(stream, SectorUnit * sector, SEEK_SET);
    fread(&neuron.type, sizeof(BYTE), 1, stream);
    if(neuron.type>>7 | 1)
    {
        if(!(neuron.type>>6 | 1))
        {
            fread(&neuron.priority, sizeof(NUMBER), 1, stream);
            fread(&neuron.size, sizeof(NUMBER), 1, stream);
            fread(&neuron.extra, sizeof(BYTE), 1, stream);
            fread(&neuron.threshold, sizeof(float), 1, stream);
            fread(&neuron.weight, sizeof(float), 1, stream);
            fread(&neuron.temp, sizeof(float), 1, stream);
            if(neuron.size > 16)
            {
                int address_size = (neuron.size - 16)>>2;//단위 BYTES[2bytes]
                BYTES bytes[address_size];
                for(int i = 0; i < address_size; i++)
                {
                    fread(&bytes[i], sizeof(BYTES), 1, stream);
                }
                //struct Neuron* n = (Neuron*) malloc(sizeof(struct Neuron) + sizeof(BYTES)*address_size);
                //n->address = bytes;
            }
        }
    }
}