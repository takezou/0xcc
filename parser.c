#include "0xcc.h"

Node *primary() {
  // if the next token is "(", it should be "(" expr ")"
  if (consume("(")) {
    Node *node = expression();
    expect(")");
    return node;
  }

  // it should be a number otherwise
  return new_node_num(expect_number());
}

Node *multiplication_or_division() {
  Node *node = unary();

  while(true) {
    if (consume("*")) {
      node = new_node(NODE_MULTIPLICATION, node, unary());
    }
    else if (consume("/")) {
      node = new_node(NODE_DIVISION, node, unary());
    }
    else {
      return node;
    }
  }
}

Node *expression() {
  Node *node = equality();
  return node;
}

Node *equality() {
  Node *node = relational();

  while(true) {
    if (consume("==")) {
      node = new_node(NODE_EQUALS, node, relational());
    }
    else if (consume("!=")) {
      node = new_node(NODE_NOT_EQUALS, node, relational());
    }
    else {
      return node;
    }
  }
}

Node *relational() {
  Node *node = addition_or_subtraction();

  if (consume("<")) {
    node = new_node(NODE_LESS_THAN, node, addition_or_subtraction());
  }
  else if (consume("<=")) {
    node = new_node(NODE_LESS_THAN_OR_EQUAL_TO, node, addition_or_subtraction());    
  }
  else if (consume(">")) {
    node = new_node(NODE_GREATER_THAN, node, addition_or_subtraction());
  }
  else if (consume(">=")) {
    node = new_node(NODE_GREATER_THAN_OR_EQUAL_TO, node, addition_or_subtraction());
  }
  return node;
}

Node *addition_or_subtraction() {
  Node *node = multiplication_or_division();

  while (true) {
    if (consume("+")) {
      node = new_node(NODE_ADDITION, node, multiplication_or_division());
    }
    else if (consume("-")) {
      node = new_node(NODE_SUBTRACTION, node, multiplication_or_division());
    }
    else {
      return node;
    }
  }
}

Node *unary() {
  if (consume("+")) {
    return primary();
  }
  if (consume("-")) {
    return new_node(NODE_SUBTRACTION, new_node_num(0), primary());
  }
  return primary();
}
//create a new token and append it to cur
Token *new_token(TokenKind kind, Token *cur, char *string) {
  Token *token = calloc(1, sizeof(Token));
  token->kind = kind;
  token->string = string;
  token->length = strlen(string);
  cur->next = token;
  return token;
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
    if(!memcmp("==", p, 2) || !memcmp("!=", p, 2) || !memcmp("<=", p, 2) || !memcmp(">=", p, 2)){
      char *token_string = calloc(3, sizeof(char));
      strncpy(token_string, p, 2);
      p+=2;
      cur = new_token(TOKEN_RESERVED, cur, token_string);
      continue;
    }
    if(!memcmp("+", p, 1) || !memcmp("-", p, 1) || !memcmp("*", p, 1) || !memcmp("/", p, 1) || !memcmp("(", p, 1) || !memcmp(")", p, 1)  || !memcmp("<", p, 1) || !memcmp(">", p, 1)) {
      char *token_string = calloc(2, sizeof(char));
      strncpy(token_string, p++, 1);
      cur = new_token(TOKEN_RESERVED, cur, token_string);
      continue;
    }
    if(isdigit(*p)) {
      cur = new_token(TOKEN_NUMBER, cur, p);
      cur->value = strtol(p, &p, 10);
      continue;
    }
    error_at(p, "cannot tokenize");
  }

  new_token(TOKEN_EOF, cur, p);
  return head.next;
}
