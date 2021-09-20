#include "DataProcess.h"

bool Load(PAGE page, SECTOR sector, Signal signal)
{
    FILE *stream = getPage(page)->stream;
    fseek(stream, SectorUnit * sector, SEEK_SET);
    BYTE ttype;
    fread(&ttype, sizeof(BYTE), 1, stream);
    
    if(ttype>>7 & 1)
    {
        if(!(ttype >> 6 & 1))
        {
            NUMBER tsize;
            fread(&tsize, sizeof(NUMBER), 1, stream);
            struct Neuron* neuron = (Neuron*) malloc(sizeof(struct Neuron) + sizeof(BYTES)*tsize);
            neuron->page = page;
            neuron->sector = sector;
            neuron->type = ttype;
            neuron->count = tsize;
            fread(&neuron->priority, sizeof(NUMBER), 1, stream);
            fread(&neuron->extra, sizeof(BYTE), 1, stream);
            fread(&neuron->threshold, sizeof(float), 1, stream);
            fread(&neuron->weight, sizeof(float), 1, stream);
            fread(&neuron->temp, sizeof(float), 1, stream);
            
            
            if(tsize > 0)
            {
                BYTES bytes[tsize];
                for(int i = 0; i < tsize; i++)
                {
                    fread(&bytes[i], sizeof(BYTES), 1, stream);
                }
            }
            return true;
        }
    }
    
    return false;
}