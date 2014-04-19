#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "utils.h"

struct TreeNode {
    char code[256];
    int freq;
    unsigned char value;
    struct TreeNode *left;
    struct TreeNode *right;
};

int doHuffman(char* infilename, char* outfilename, int mode) {
    FILE *infile, *outfile;
    if (!(infile = fopen(infilename, "rb"))) {
        return 1;
    }
    if (!(outfile = fopen(outfilename, "wb"))) {
        return 2;
    }
    if (mode == 0) {
        encode(infile, outfile);
    } else {
        decode(infile, outfile);
    }
    fclose(infile);
    fclose(outfile);
    return 0;
}

void encode(FILE *infile, FILE *outfile) {
    char *curCode;
    int len, i;
    Tree *tree;

    fseek(infile, 0L, SEEK_END);
    len = ftell(infile);
    rewind(infile);
    tree = generateHuffmanTree(infile);
    rewind(infile);
    writeHeader(outfile, tree, len);
    for (i = 0; i < len; i++) {
        curCode = getCode(tree, fgetc(infile));
        writeCode(outfile, convertCode(curCode), strlen(curCode));
    }

    writeCode(outfile, 0, 8 - getCount());

    destroyTree(tree);
}

void decode(FILE *infile, FILE *outfile) {
    char *curCode;
    int resByte, buffer;
    int i, bit, codeLen = 0, writtenLen = 0, length = 0;
    Tree *tree;

    readHeader(infile, &tree, &length);
    setCount(0);
    for (i = 0; i < length; i++) {
        int ch = decodeByte(infile, tree);
        fputc(ch, outfile);
    }
    destroyTree(tree);
}



void writeHeader(FILE *file, Tree *tree, int length) {
    writeInt(file, length);
    writeHuffmansTree(file, tree);
    writeCode(file, 0, 8 - getCount());
}

void readHeader(FILE *file, Tree **tree, int *length) {
    *length = readInt(file);
    *tree = readHuffmansTree(file);
    generateCodes(*tree);
}

void writeHuffmansTree(FILE* file, Tree *tree) {
    // Нет нужды проверять каждое поддерево, т.к. возможны всего 2 случая: узел с двумя поддеревьями или лист
    if (!tree->left && !tree->right) {
        writeBit(file, 1);
        writeCode(file, tree->value, 8);
    } else {
        writeBit(file, 0);
        writeHuffmansTree(file, tree->left);
        writeHuffmansTree(file, tree->right);
    }
}

Tree* readHuffmansTree(FILE* file) {
    Tree *node = (Tree*) malloc(sizeof(Tree));

    if (readBit(file)) {
        node->value = readByte(file);
        node->code[0] = 0;
        node->left = NULL;
        node->right = NULL;
    } else {
        node->value = 0;
        node->code[0] = 0;
        node->left = readHuffmansTree(file);
        node->right = readHuffmansTree(file);
    }
    return node;
}

int decodeByte(FILE* in, Tree *tree) {
  if (tree->value != 0) { // Лист
    return tree->value;
  } else if (readBit(in)) {
      return decodeByte(in, tree->right);
  } else {
      return decodeByte(in, tree->left);
  }
}

char *getCode(Tree* tree, unsigned char c) {
    if (tree != NULL) {
        if (tree->value == c) {
            return tree->code;
        } else {
            if (getCode(tree->left, c) != NULL) {
                return getCode(tree->left, c);
            }
            if (getCode(tree->right, c) != NULL) {
                return getCode(tree->right, c);
            }
        }
    }
    return NULL;
}

Tree* generateHuffmanTree(FILE *infile) {
    int *joins = (int*) calloc(256, sizeof(int));
    int notNull = 0, freq, i;
    unsigned char c;

    while ((c = (unsigned char) fgetc(infile)) != (unsigned char) EOF) {
        joins[(int) c]++;
    }
    for (i = 0; i < 256; i++) {
        if (joins[i] != 0) notNull++;
    }
    Tree **nodes = malloc(notNull * sizeof(Tree*));
    Tree *root;

    for (i = 0; i < notNull; i++) {
        c = findMax(joins, 256, &freq);
        Tree *node = (Tree*) malloc(sizeof(Tree));
        node->freq = freq;
        node->left = NULL;
        node->right = NULL;
        node->value = c;
        nodes[i] = node;
    }

    root = linkTreeNodes(nodes, notNull);
    free(joins);
    free(nodes);
    generateCodes(root);
    return root;
}

Tree* linkTreeNodes(Tree *nodes[], int k) {
    int i, j;
    Tree *node = (Tree*) malloc(sizeof(Tree));
    node->freq = nodes[k - 1]->freq + nodes[k - 2]->freq;
    node->value = (unsigned char) 0;
    node->code[0] = 0;
    node->left = nodes[k - 1];
    node->right = nodes[k - 2];

    if (k == 2) {
        return node;
    } else {
        // Вставляем в нужное место получившийся узел
        for (i = 0; i < k; i++)
            if (node->freq > nodes[i]->freq) {
                for (j = k - 1; j > i; j--) {
                    nodes[j] = nodes[j - 1];
                }
                nodes[i] = node;
                break;
            }
    }
    return linkTreeNodes(nodes, k - 1);
}

void generateCodes(Tree *root) {
    if (root->left != NULL) {
        strcpy(root->left->code, root->code);
        strcat(root->left->code, "0");
        generateCodes(root->left);
        // if (root->left->value != '\0') printf("Code of \'%d = %c\' is now: %s\n", root->left->value, root->left->value, root->left->code);
    }
    if (root->right != NULL) {
        strcpy(root->right->code, root->code);
        strcat(root->right->code, "1");
        generateCodes(root->right);
        // if (root->right->value != '\0') printf("Code of \'%d = %c\' is now: %s\n", root->right->value, root->right->value, root->right->code);
    }
}

void destroyTree(Tree *tree) {
    Tree *left;
    Tree *right;
    if (tree) {
        left = tree->left;
        right = tree->right;
        free(tree);
        destroyTree(left);
        destroyTree(right);
    }
}
