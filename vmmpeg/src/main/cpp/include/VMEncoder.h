//
// Created by lz liu on 2018/5/10.
//

#include <stdint.h>

#ifndef VMLIBRARYMANAGER_VMENCODER_H
#define VMLIBRARYMANAGER_VMENCODER_H

int initEncoder(const char *outPath, int width, int height, int degrees);

int encodeData(uint8_t *data, int width, int height, int degrees);

int freeEncoder();

int yuv2Bitmap(uint8_t *data, int width, int height, int degrees, const char *outPath);


#endif //VMLIBRARYMANAGER_VMENCODER_H
