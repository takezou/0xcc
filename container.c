#include "0xcc.h"

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

bool consume(char *operator) {
  if (token->kind != TOKEN_RESERVED || strlen(operator) != token->length || memcmp(token->string, operator, token->length)) {
    return false;
  }
  token = token->next;
  return true;
}

Token *consume_identifier() {
  if (token->kind != TOKEN_IDENTIFIER) {
    return false;
  }
  Token *current_token = token;
  token = token->next;
  return current_token;
}

// if next token is an expected one, read 1 more token ahead and return true
// report error otherwise
void expect(char *operator) {
  if (token->kind != TOKEN_RESERVED || strlen(operator) != token->length || memcmp(token->string, operator, token->length)) {
    error_at(token->string, "missing '%s' here", operator);
  }
  token = token->next;
}

// if next token is a number, read one token and return the value
// report an error otherwise
int expect_number() {
  if (token->kind != TOKEN_NUMBER) {
    error_at(token->string, "missing a number here");
  }
  int value = token->value;
  token = token->next;
  return value;
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

Node *new_node_num(int value) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NODE_NUMBER;
  node->value = value;
  return node;
}
