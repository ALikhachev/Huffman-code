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
    if (!(infile = fopen(infilename, mode == 0 ? "r" : "rb"))) {
        return 1;
    }
    if (!(outfile = fopen(outfilename, mode == 0 ? "wb" : "w"))) {
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
    char *input = initString(), *curCode;
    char readbuffer = 0;
    int len;
    Tree *tree;

    while((readbuffer = fgetc(infile)) != EOF) {
        input = appendCharToArray(input, readbuffer);
    }
    len = strlen(input);
    tree = generateHuffmanTree(input);
    writeHeader(outfile, tree, len);
    for (int i = 0; i < len; i++) {
        curCode = getCode(tree, input[i]);
        writeCode(outfile, convertCode(curCode), strlen(curCode));
    }

    writeCode(outfile, 0, 8 - getCount());

    free(input);

    destroyTree(tree);
}

void decode(FILE *infile, FILE *outfile) {
    char *curCode = initString();
    char resByte, buffer;
    int bit, codeLen = 0, writtenLen = 0, limit = 0;
    Tree *tree;

    readHeader(infile, &tree, &limit);
    while ((buffer = fgetc(infile)) != EOF) {
        bit = 0;
        while (bit < 8) {
            if (codeLen++ > 32) {
                printf("May be Incorrect input?\n");
            }
            curCode = appendCharToArray(curCode, getBit(buffer, 7 - bit++));
            resByte = findCharByCode(tree, curCode);
            if (resByte) {
                codeLen = 0;
                fputc(resByte, outfile);
                free(curCode);
                curCode = initString();
                if (++writtenLen >= limit) break;
            }
        }
    }

    free(curCode);
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

char findCharByCode(Tree* tree, char* code) {
    if (tree != NULL) {
        if (strcmp(tree->code, code) == 0) {
            return tree->value;
        } else {
            if (findCharByCode(tree->left, code) != 0) {
                return findCharByCode(tree->left, code);
            }
            if (findCharByCode(tree->right, code) != 0) {
                return findCharByCode(tree->right, code);
            }
        }
    }
    return 0;
}

char *getCode(Tree* tree, char c) {
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

Tree* generateHuffmanTree(char* input) {
    int *joins = (int*) calloc(256, sizeof(int));
    int notNull = 0;

    countJoins(input, joins);
    for (int i = 0; i < 256; i++) {
        if (joins[i] != 0) notNull++;
    }
    Tree **nodes = malloc(notNull * sizeof(Tree*));
    Tree *root;

    for (int i = 0; i < notNull; i++) {
        int freq;
        char c = findMax(joins, 256, &freq);
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
    Tree *node = (Tree*) malloc(sizeof(Tree));
    node->freq = nodes[k - 1]->freq + nodes[k - 2]->freq;
    node->value = 0;
    node->code[0] = 0;
    node->left = nodes[k - 1];
    node->right = nodes[k - 2];

    if (k == 2) {
        return node;
    } else {
        // Вставляем в нужное место получившийся узел
        for (int i = 0; i < k; i++)
            if (node->freq > nodes[i]->freq) {
                for (int j = k - 1; j > i; j--) {
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

void countJoins(char *input, int *joins) {
    for (int i = 0; i < strlen(input); i++) {
        joins[(int)input[i]]++;
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
