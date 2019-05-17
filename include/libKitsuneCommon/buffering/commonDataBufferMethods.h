#ifndef COMMONDATABUFFERMETHODS_H
#define COMMONDATABUFFERMETHODS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

namespace Kitsune
{
struct CommonDataBuffer;


void addDataToBuffer(CommonDataBuffer *buffer,
                     uint8_t *data,
                     const uint64_t size);
bool allocateBlocks(CommonDataBuffer *buffer,
                    const uint32_t numberOfBlocks);
void resetBuffer(CommonDataBuffer *buffer);
}

#endif // COMMONDATABUFFERMETHODS_H
