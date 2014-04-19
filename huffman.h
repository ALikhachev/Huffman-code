#ifndef HUFFMAN_H_INCLUDED
#define HUFFMAN_H_INCLUDED
#include "utils.h"

/**
  * Tree node of Huffman's tree.
  * Contains following fields:
  *  - unsigned char value = char in that node;
  *  - char *code = code of char;
  *  - int freq = frequency of char in input string;
  *  - struct TreeNode *left = pointer to the left sub-node;
  *  - struct TreeNode *right = pointer to the right sub-node;
  */
typedef struct TreeNode Tree;

/**
  * Performs encoding and decoding (according to first arg: 0 = encode, 1 = decode) by Huffman's algorithm.
  *
  * Args: char array, mode.
  */
int doHuffman(char*, char*, int);

/**
  * Performs encoding using generated Huffman's tree.
  *
  * Args: pointer to the root of Huffmans tree, string to encode.
  */
void encode(FILE*, FILE*);

/**
  * Performs decoding using generated Huffman's tree.
  *
  * Args: pointer to the root of Huffmans tree, string to decode, length of original string.
  */
void decode(FILE*, FILE*);

/**
  * Generates Huffman's tree (for encoding).
  *
  * Args: string to encode, length of input.
  */
Tree* generateHuffmanTree(FILE*, int);

/**
  * auxiliary function
  */
Tree* linkTreeNodes(Tree*[], int);

/**
  * Returns char by it's code in Huffman's tree.
  *
  * Args: pointer to the file, pointer to the root of Huffmans tree.
  */
int decodeByte(FILE*, Tree*);

/**
  * Returns code for char in Huffman's tree.
  *
  * Args: pointer to the root of Huffmans tree, char.
  */
char* getCode(Tree*, unsigned char);

/**
  * Generates codes for all nodes of Huffman's tree (left = 0, right = 1)
  *
  * Args: pointer to the root of Huffmans tree.
  */
void generateCodes(Tree*);

/**
  * Writes Huffman's tree to file as sequence of bits.
  * If the node is leaf then write one bit '1' and the 8 bits of value else write one bit '0'.
  *
  * Args: pointer to file, pointer to Huffman's tree.
  */
void writeHuffmansTree(FILE*, Tree*);

/**
  * Restores Huffman's tree that written in 'writeHuffmansTree' function.
  *
  * Args: pointer to file.
  */
Tree* readHuffmansTree(FILE*);

/**
  * Writes number of symbols in decoded string and Huffman's tree to file.
  *
  * Args: pointer to file, pointer to Huffman's tree, number of symbols.
  */
void writeHeader(FILE*, Tree*, int);

/**
  * Reads number of symbols in decoded string and Huffman's tree from file.
  *
  * Args: pointer to file, pointer to pointer to Huffman's tree, pointer to number of symbols.
  */
void readHeader(FILE*, Tree**, int*);

/**
  * Destroys Huffmans tree and releases all the memory that allocated to it.
  *
  * Args: pointer to pointer to Huffman's tree.
  */
void destroyTree(Tree*);

#endif // HUFFMAN_H_INCLUDED
