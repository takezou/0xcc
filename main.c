#include "0xcc.h"

int main (int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Wrong number of arguments.\nUsage: %s <number>\n", argv[0]);
    return 1;
  }

  // tokenize and parse input data
  user_input = argv[1];
  token = tokenize();
  Node *node = expression();

  // print first half of assembly code
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // traverse abstract syntax tree and generate code
  generate(node);

  // resulting value of evaluating whole expression should be at the top of the stack
  // pop the value to RAX and return the value from the main function
  printf("  pop rax\n");
  printf("  ret\n");
  
  return 0;
}
