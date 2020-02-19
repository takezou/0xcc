#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// type of node in abstract syntax tree
typedef enum {
  NODE_ADDITION, // +
  NODE_SUBTRACTION, // -
  NODE_MULTIPLICATION, // *
  NODE_DIVISION, // /
  NODE_NUMBER, // number
} NodeKind;

typedef struct Node Node;

// abstract syntax tree node type
struct Node {
  NodeKind kind; // node type
  Node *lhs; // left hand side
  Node *rhs; // right hand side
  int val; // used only when kind is NODE_NUMBER
};

// Token kind
typedef enum {
  TOKEN_RESERVED, // symbol token
  TOKEN_NUMBER, // integer token
  TOKEN_EOF // token to designate end of input
} TokenKind;

typedef struct Token Token;

// Token type
struct Token {
  TokenKind kind; // token type
  Token *next; // next input token
  int val; // value if token kind is TOKEN_NUMBER
  char *str; // token string
};

// token currently looked at
Token *token;

// input
char *user_input;


Node *primary();
Node *multiplication_or_division();
Node *expression();
Node *unary();

// report error and its location
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " "); // print space with pos length
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// function for error reporting
// takes same arguments as printf
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool consume(char op) {
  if(token->kind != TOKEN_RESERVED || token->str[0] != op) {
    return false;
  }
  token = token->next;
  return true;
}

// if next token is an expected one, read 1 more token ahead and return true
// report error otherwise
void expect(char op) {
  if (token->kind != TOKEN_RESERVED || token->str[0] != op) {
    error_at(token->str, "missing '%c' here", op);
  }
  token = token->next;
}

// if next token is a number, read one token and return the value
// report an error otherwise
int expect_number() {
  if (token->kind != TOKEN_NUMBER) {
    error_at(token->str, "missing a number here");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TOKEN_EOF;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_NUMBER;
  node->val = val;
  return node;
}

Node *primary() {
  // if the next token is "(", it should be "(" expr ")"
  if (consume('(')) {
    Node *node = expression();
    expect(')');
    return node;
  }

  // it should be a number otherwise
  return new_node_num(expect_number());
}

Node *multiplication_or_division() {
  Node *node = unary();

  while(true) {
    if (consume('*')) {
      node = new_node(NODE_MULTIPLICATION, node, unary());
    }
    else if (consume('/')) {
      node = new_node(NODE_DIVISION, node, unary());
    }
    else {
      return node;
    }
  }
}

Node *expression() {
  Node *node = multiplication_or_division();

  while (true) {
    if (consume('+')) {
      node = new_node(NODE_ADDITION, node, multiplication_or_division());
    }
    else if (consume('-')) {
      node = new_node(NODE_SUBTRACTION, node, multiplication_or_division());
    }
    else {
      return node;
    }
  }
}

Node *unary() {
  if (consume('+')) {
    return primary();
  }
  if (consume('-')) {
    return new_node(NODE_SUBTRACTION, new_node_num(0), primary());
  }
  return primary();
}
//create a new token and append it to cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// tokenize input string p and return it
Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*p) {
    // skip whitespaces
    if(isspace(*p)) {
      p++;
      continue;
    }

    if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
      cur = new_token(TOKEN_RESERVED, cur, p++);
      continue;
    }

    if(isdigit(*p)) {
      cur = new_token(TOKEN_NUMBER, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    error_at(p, "cannot tokenize");
  }

  new_token(TOKEN_EOF, cur, p);
  return head.next;
}

void generate(Node *node) {
  if (node->kind == NODE_NUMBER) {
    printf("  push %d\n", node->val);
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
  }

  printf("  push rax\n");
}

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
