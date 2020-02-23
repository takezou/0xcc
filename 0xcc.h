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
  NODE_EQUALS, // ==
  NODE_NOT_EQUALS, // !=
  NODE_LESS_THAN, // <
  NODE_LESS_THAN_OR_EQUAL_TO, // <=
  NODE_GREATER_THAN, // >
  NODE_GREATER_THAN_OR_EQUAL_TO, // >=
  NODE_NUMBER, // number
} NodeKind;

typedef struct Node Node;

// abstract syntax tree node type
struct Node {
  NodeKind kind; // node type
  Node *lhs; // left hand side
  Node *rhs; // right hand side
  int value; // used only when kind is NODE_NUMBER
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
  int value; // value if token kind is TOKEN_NUMBER
  char *string; // token string
  int length; // token length
};

// token currently looked at
Token *token;

// input
char *user_input;


Node *primary();
Node *multiplication_or_division();
Node *expression();
Node *unary();
Node *equality();
Node *relational();
Node *addition_or_subtraction();

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
bool consume(char *operator);
void expect(char *operator);
int expect_number();
bool at_eof();

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int value);
Node *primary();
Node *multiplication_or_division();
Node *expression();
Node *equality();
Node *relational();
Node *addition_or_subtraction();
Node *unary();

Token *new_token(TokenKind kind, Token *cur, char *string);
Token *tokenize();

void generate(Node *node);