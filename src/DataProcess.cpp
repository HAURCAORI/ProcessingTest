#include "DataProcess.h"

bool Load(PAGE page, SECTOR sector, Signal signal)
{
    FILE *stream = getPage(page)->stream;

    Neuron neuron;
    fseek(stream, NeuronHeaderSize * sector, SEEK_SET);

}