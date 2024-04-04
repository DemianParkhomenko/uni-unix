#include <stdio.h>
# 216 "./some-file.h"

// Preprocessor can create comments for compiler.
// For examples, path to binary files or header.

// Preprocessing
// `gcc -E ./compiler-comments.c > compiler.preprocessed`

// Compilation
// `gcc -c ./compiler.preprocessed`

//* Show object file in hex
//*  hexdump -C
//* /home/demian/KPI/code/c/code-examples/compiler/compiler-preprocessed.o

//* Show object file in bin
//* xxd -b
//* /home/demian/KPI/code/c/code-examples/compiler/compiler-preprocessed.o

// link object file with libs and other files
// gcc compiler-preprocessed.o -o compiler.out

int main(int argc, char const *argv[]) {
  printf("Hello world \n");
  return 0;
}
