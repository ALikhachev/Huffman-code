#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

/**
  * Finds max element of integer array and index of that element.
  *
  * Args: int array, array's size, pointer to int (to write index of max element).
  */
char findMax(int*, int, int*);

/**
  * Converts code from string representation to int.
  *
  * Args: Code of a symbol as char sequence.
  */
int convertCode(char*);

/**
  * For internal use (sets the buffer counter).
  *
  */
void setCount(int);

/**
  * For internal use (returns value of the buffer counter).
  *
  */
int getCount();

/**
  * Writes one bit to file.
  *
  * Args: pointer to file, int 0 or 1 appropriate to bit value.
  */
void writeBit(FILE*, int);

/**
  * Reads one bit from file.
  *
  * Args: pointer to file.
  */
int readBit(FILE*);

/**
  * Reads 8 bits from file.
  *
  * Args: pointer to file.
  */
int readByte(FILE*);

/**
  * Writes specified number of bits to file.
  *
  * Args: pointer to file, sequence of bits in int, number of bits.
  */
void writeCode(FILE*, int, int);

/**
  * Returns a specified bit of specified byte (as characters '0' and '1').
  *
  * Args: byte, offset (from right border).
  */
char getBit(unsigned char, int);

/**
  * Initiate empty string (char array).
  *
  * Args: no.
  */
char* initString();

/**
  * Append char to char array.
  *
  * Args: char array, char to append.
  */
char* appendCharToArray(const char*, char);

/**
  * Writes 4 bytes of integer to file.
  *
  * Args: pointer to file, integer number.
  */
void writeInt(FILE*, int);

/**
  * Reads 4 bytes of integer from file.
  *
  * Args: pointer to file.
  */
int readInt(FILE*);

#endif // UTILS_H_INCLUDED
