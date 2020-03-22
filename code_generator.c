#include "0xcc.h"

void generate_local_variable(Node *node) {
  if (node->kind != NODE_LOCAL_VARIABLE) {
    error("Left value of the assignment is not a local variable.");
  }
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void generate(Node *node) {
  if(!node) return;
  switch (node->kind) {
  case NODE_NUMBER:
    printf("  push %d\n", node->value);
    return;
    
  case NODE_LOCAL_VARIABLE:
    generate_local_variable(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;

  case NODE_ASSIGNMENT:
    generate_local_variable(node->lhs);
    generate(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;

  case NODE_RETURN:
    generate(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;

  case NODE_IF:
    generate(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .LendXXX\n");
    generate(node->rhs);
    printf(".LendXXX:\n");
    return;
  }

  generate(node->lhs);
  generate(node->rhs);
  
  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case NODE_ADDITION:
    printf("  add rax, rdi\n");
    break;
    
  case NODE_SUBTRACTION:
    printf("  sub rax, rdi\n");
    break;
    
  case NODE_MULTIPLICATION:
    printf("  imul rax, rdi\n");
    break;
    
  case NODE_DIVISION:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;

  case NODE_EQUALS:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;

  case NODE_NOT_EQUALS:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;

  case NODE_LESS_THAN:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
    
  case NODE_LESS_THAN_OR_EQUAL_TO:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;

  case NODE_GREATER_THAN:
    printf("  cmp rax, rdi\n");
    printf("  setg al\n");
    printf("  movzb rax, al\n");
    break;

  case NODE_GREATER_THAN_OR_EQUAL_TO:
    printf("  cmp rax, rdi\n");
    printf("  setge al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
}
