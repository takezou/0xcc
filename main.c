#include "0xcc.h"

int main (int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Wrong number of arguments.\nUsage: %s <number>\n", argv[0]);
    return 1;
  }

  // tokenize and parse input data
  user_input = argv[1];
  token = tokenize();
  //Node *node = expression();
  program();
  
  // print first half of assembly code
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // prologue
  // allocate space for 26 variables
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  // generate code for each statement from beginning
  for (int i = 0; code[i] != NULL; i++) {
    // traverse abstract syntax tree and generate code
    generate(code[i]);

    // a resulting value of evaluating a statement should be on a stack
    // pop it to RAX so that the stack doesn't overflow
    printf("  pop rax\n");
  }
  
  // result of evaluating the last expression should be in RAX, and it is returned from main
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  
  return 0;
}
