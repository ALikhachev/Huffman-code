#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

unsigned char buffer = 0; // Буфер вывода
int count = 0; // Кол-во бит в буфере

void writeBit(FILE* out, int value) {
    buffer = buffer << 1;
    if (value) {
        buffer |= 1;
    }
    count++;
    if (count == 8) {
        fwrite(&buffer, 1, 1, out);
        count = 0;
    }
}

void setCount(int v) {
    count = v;
}

int getCount() {
    return count;
}

void writeCode(FILE* out, int code, int length) {
    int i, mask;
    for (i = length; i > 0; i--) {
        mask = 1 << (i - 1);
        writeBit(out, code & mask);
    }
}

int convertCode(char *code) {
    int i, res = 0;
    for (i = 0; i < strlen(code); i++) {
        res = res << 1;
        if (code[i] == '1') {
            res |= 1;
        }
    }
    return res;
}

int readBit(FILE* in) {
    if (count == 0) {
        int readed = fread(&buffer, 1, 1, in);
        count = readed * 8;
    }
    int res = buffer & 0x80; // Выбираем старший бит
    buffer = buffer << 1;
    count--;
    return res;
}

int readByte(FILE* in) {
    int i, res = 0;
    for (i = 0; i < 8; i++) {
        res = res << 1;
        if (readBit(in)) {
            res |= 1;
        }
    }
    return res;
}

char getBit(unsigned char byte, int offset) {
    return (((byte >> offset) & 0x1) == 0x1) ? '1' : '0';
}

char findMax(int *array, int size, int *frequency) {
    int i, max = array[0], maxIndex = 0;
    for (i = 0; i < size; i++) {
        if (array[i] > max) {
            max = array[i];
            maxIndex = i;
        }
    }
    array[maxIndex] = 0; // значение обнуляется, чтобы один и тот же символ не встречался более одного раза
    if (max != 0) *frequency = max;
    return (char) maxIndex;
}

void writeInt(FILE *file, int value) {
    fwrite(&value, sizeof(int), 1, file);
}

int readInt(FILE *file) {
    int value;
    fread(&value, sizeof(int), 1, file);
    return value;
}
