#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tokey kind
typedef enum {
  TK_RESERVED, // symbol token
  TK_NUM, // integer token
  TK_EOF // token to designate end of input
} TokenKind;

typedef struct Token Token;

// Token type
struct Token {
  TokenKind kind; // token type
  Token *next; // next input token
  int val; // value if token kind is TK_NUM
  char *str; // token string
};

// token currently looked at
Token *token;

// input
char *user_input;

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
  if(token->kind != TK_RESERVED || token->str[0] != op) {
    return false;
  }
  token = token->next;
  return true;
}

// if next token is an expected one, read 1 more token ahead and return true
// report error otherwise
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) {
    error_at(token->str, "missing '%c' here", op);
  }
  token = token->next;
}

// if next token is a number, read one token and return the value
// report an error otherwise
int expect_number() {
  if (token->kind != TK_NUM) {
    error_at(token->str, "missing a number here");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
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
Token *nnntokenize() {
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

    if(*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if(isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    error_at(p, "cannot tokenize");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main (int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Wrong number of arguments.\nUsage: %s <number>\n", argv[0]);
    return 1;
  }

  user_input = argv[1];

  // tokenize input data
  token = tokenize();

  // print first half of assembly code
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // expression must start with a number, so check that and
  // print first mov instruction
  printf("  mov rax, %d\n", expect_number());

  // consume `+ <number>` or `- <number>` inputs and
  // print assembly instructions
  while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  
  return 0;
}
