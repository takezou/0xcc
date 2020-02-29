#include "0xcc.h"

Node *primary() {
  // if the next token is "(", it should be "(" expr ")"
  if (consume("(")) {
    Node *node = expression();
    expect(")");
    return node;
  }

  // local variable
  Token *token = consume_identifier();
  if (token) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = NODE_LOCAL_VARIABLE;
    node->offset = (token->string[0] - 'a' + 1) * 8;
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
  return assignment();
}

Node *statement() {
  Node *node = expression();
  expect(";");
  return node;
}

void program() {
  int i = 0;
  while (!at_eof()) {
    code[i++] = statement();
  }
  code[i] = NULL;
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

Node *assignment() {
  Node *node = equality();
  if (consume("=")) {
    node = new_node(NODE_ASSIGNMENT, node, assignment());
  }
  return node;
}

//create a new token and append it to cur
Token *new_token(TokenKind kind, Token *current_token, char *string) {
  Token *token = calloc(1, sizeof(Token));
  token->kind = kind;
  token->string = string;
  token->length = strlen(string);
  current_token->next = token;
  return token;
}

// tokenize input string p and return it
Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *current_token = &head;

  while (*p) {
    // skip whitespaces
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (!memcmp("==", p, 2) || !memcmp("!=", p, 2) || !memcmp("<=", p, 2) || !memcmp(">=", p, 2)) {
      char *token_string = calloc(3, sizeof(char));
      strncpy(token_string, p, 2);
      p+=2;
      current_token = new_token(TOKEN_RESERVED, current_token, token_string);
      continue;
    }
    if ('a' <= *p && *p <= 'z') {
      int name_length = strspn(p, "abcdefghijklmnopqrstuvwxyz");
      char *variable_name = calloc(name_length + 1, sizeof(char));
      strncpy(variable_name, p, name_length);
      current_token = new_token(TOKEN_IDENTIFIER, current_token, variable_name);
      p += name_length;
      continue;
    }
    if (!memcmp("+", p, 1) || !memcmp("-", p, 1) || !memcmp("*", p, 1) || !memcmp("/", p, 1) || !memcmp("(", p, 1) || !memcmp(")", p, 1)  || !memcmp("<", p, 1) || !memcmp(">", p, 1) || !memcmp("=", p, 1) || !memcmp(";", p, 1)) {
      char *token_string = calloc(2, sizeof(char));
      strncpy(token_string, p++, 1);
      current_token = new_token(TOKEN_RESERVED, current_token, token_string);
      continue;
    }
    if(isdigit(*p)) {
      current_token = new_token(TOKEN_NUMBER, current_token, p);
      current_token->value = strtol(p, &p, 10);
      continue;
    }
    error_at(p, "cannot tokenize");
  }

  new_token(TOKEN_EOF, current_token, p);
  return head.next;
}
