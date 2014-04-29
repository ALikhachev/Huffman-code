#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "huffman.h"

const char *messages[] =
{
  "Input successfully processed.\n",
  "Can't open input file to read.\n",
  "Can't open output file to write.\n"
};

int main(int argc, char *argv[])
{
  int mode;
  char *infilename;
  char *outfilename;
  mode = (argc > 1) ? (!strcmp(argv[1], "e") ? 0 : 1) : 0;
  infilename = argc > 2 ? argv[2] : "input.txt";
  outfilename = argc > 3 ? argv[3] : "output.txt";
  setlocale(LC_ALL, "");
  printf("Input file: %s\n", infilename);
  printf("Output file: %s\n", outfilename);
  printf("Mode: %s\n\n", mode == 0 ? "encoding" : "decoding");

  printf("%s", messages[doHuffman(infilename, outfilename, mode)]);

  return 0;
}
