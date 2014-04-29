#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "utils.h"

struct TreeNode
{
  char code[256];
  int freq;
  int value;
  struct TreeNode *left;
  struct TreeNode *right;
};

int doHuffman(char*, char*, int);
void encode(FILE*, FILE*);
void decode(FILE*, FILE*);
Tree* generateHuffmanTree(FILE*, int);
Tree* linkTreeNodes(Tree*[], int);
int decodeByte(FILE*, Tree*);
char* getCode(Tree*, unsigned char);
void generateCodes(Tree*);
void writeHuffmansTree(FILE*, Tree*);
Tree* readHuffmansTree(FILE*);
void writeHeader(FILE*, Tree*, int);
void readHeader(FILE*, Tree**, int*);
void destroyTree(Tree*);

int doHuffman(char* infilename, char* outfilename, int mode)
{
  FILE *infile, *outfile;
  if (!(infile = fopen(infilename, "rb")))
    {
      return 1;
    }
  if (!(outfile = fopen(outfilename, "wb")))
    {
      return 2;
    }
  if (mode == 0)
    {
      encode(infile, outfile);
    }
  else
    {
      decode(infile, outfile);
    }
  fclose(infile);
  fclose(outfile);
  return 0;
}

void encode(FILE *infile, FILE *outfile)
{
  char *curCode;
  int len, i;
  Tree *tree;

  fseek(infile, 0L, SEEK_END);
  len = ftell(infile);
  rewind(infile);
  tree = generateHuffmanTree(infile, len);
  rewind(infile);
  writeHeader(outfile, tree, len);
  for (i = 0; i < len; i++)
    {
      curCode = getCode(tree, (unsigned char) fgetc(infile));
      writeCode(outfile, convertCode(curCode), strlen(curCode));
    }
  writeCode(outfile, 0, 8 - getCount());

  destroyTree(tree);
}

void decode(FILE *infile, FILE *outfile)
{
  int i, ch, length = 0;
  Tree *pTree;

  readHeader(infile, &pTree, &length);
  setCount(0);
  for (i = 0; i < length; i++)
    {
      ch = decodeByte(infile, pTree);
      fputc(ch, outfile);
    }
  destroyTree(pTree);
}



void writeHeader(FILE *file, Tree *pTree, int length)
{
  writeInt(file, length);
  writeHuffmansTree(file, pTree);
  writeCode(file, 0, 8 - getCount());
}

void readHeader(FILE *file, Tree **ppTree, int *length)
{
  *length = readInt(file);
  *ppTree = readHuffmansTree(file);
  generateCodes(*ppTree);
}

void writeHuffmansTree(FILE* file, Tree *pTree)
{
  // Нет нужды проверять каждое поддерево, т.к. возможны всего 2 случая: узел с двумя поддеревьями или лист
  if (!pTree->left && !pTree->right)
    {
      writeBit(file, 1);
      writeCode(file, pTree->value, 8);
    }
  else
    {
      writeBit(file, 0);
      writeHuffmansTree(file, pTree->left);
      writeHuffmansTree(file, pTree->right);
    }
}

Tree* readHuffmansTree(FILE* file)
{
  Tree *node = (Tree*) malloc(sizeof(Tree));

  if (readBit(file))
    {
      node->value = readByte(file);
      node->code[0] = 0;
      node->left = NULL;
      node->right = NULL;
    }
  else
    {
      node->value = -1;
      node->code[0] = 0;
      node->left = readHuffmansTree(file);
      node->right = readHuffmansTree(file);
    }
  return node;
}

int decodeByte(FILE* in, Tree *tree)
{
  if (tree->value != -1)   // Лист
    {
      return tree->value;
    }
  else if (readBit(in))
    {
      return decodeByte(in, tree->right);
    }
  else
    {
      return decodeByte(in, tree->left);
    }
}

char *getCode(Tree* tree, unsigned char c)
{
  if (tree != NULL)
    {
      if (tree->value == c)
        {
          return tree->code;
        }
      else
        {
          if (getCode(tree->left, c) != NULL)
            {
              return getCode(tree->left, c);
            }

          if (getCode(tree->right, c) != NULL)
            {
              return getCode(tree->right, c);
            }
        }
    }
  return NULL;
}

Tree* generateHuffmanTree(FILE *infile, int len)
{
  int *joins;
  int notNull, freq, i;
  unsigned char c;
  Tree **nodes;
  Tree *root;
  Tree *node;

  i = 0, joins = (int*) calloc(256, sizeof(int));;
  while (i++ < len)
    {
      joins[(int) fgetc(infile)]++;
    }

  notNull = 0;
  for (i = 0; i < 256; i++)
    {
      if (joins[i] != 0) notNull++;
    }

  nodes = malloc(notNull * sizeof(Tree*));

  for (i = 0; i < notNull; i++)
    {
      c = findMax(joins, 256, &freq);
      node = (Tree*) malloc(sizeof(Tree));
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

Tree* linkTreeNodes(Tree *nodes[], int k)
{
  int i, j;
  Tree *node;
  node = (Tree*) malloc(sizeof(Tree));
  node->freq = nodes[k - 1]->freq + nodes[k - 2]->freq;
  node->value = -1;
  node->code[0] = 0;
  node->left = nodes[k - 1];
  node->right = nodes[k - 2];

  if (k == 2)
    {
      return node;
    }
  else
    {
      // Вставляем в нужное место получившийся узел
      for (i = 0; i < k; i++)
        if (node->freq > nodes[i]->freq)
          {
            for (j = k - 1; j > i; j--)
              {
                nodes[j] = nodes[j - 1];
              }
            nodes[i] = node;
            break;
          }
    }
  return linkTreeNodes(nodes, k - 1);
}

void generateCodes(Tree *root)
{
  if (root->left != NULL)
    {
      strcpy(root->left->code, root->code);
      strcat(root->left->code, "0");
      generateCodes(root->left);
      // if (root->left->value != '\0') printf("Code of \'%d = %c\' is now: %s\n", root->left->value, root->left->value, root->left->code);
    }
  if (root->right != NULL)
    {
      strcpy(root->right->code, root->code);
      strcat(root->right->code, "1");
      generateCodes(root->right);
      // if (root->right->value != '\0') printf("Code of \'%d = %c\' is now: %s\n", root->right->value, root->right->value, root->right->code);
    }
}

void destroyTree(Tree *tree)
{
  Tree *left;
  Tree *right;
  if (tree)
    {
      left = tree->left;
      right = tree->right;
      free(tree);
      destroyTree(left);
      destroyTree(right);
    }
}
