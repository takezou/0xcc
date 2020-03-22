#include "0xcc.h"

LocalVariable *find_local_variable(Token *token) {
  for (LocalVariable *variable = locals; variable != NULL; variable = variable->next) {
    if (variable->length == token->length && !memcmp(token->string, variable->name, variable->length)) {
      return variable;
    }
  }
  return NULL;
}

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

    LocalVariable *local_variable = find_local_variable(token);
    if (local_variable != NULL) {
      node->offset = local_variable->offset;
    } else {
      local_variable = calloc(1, sizeof(LocalVariable));
      local_variable->next = locals;
      local_variable->name = token->string;
      local_variable->length = token->length;
      local_variable->offset += 8 + (locals != NULL ? locals->offset : 0);
      node->offset = local_variable->offset;
      locals = local_variable;
    }
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
  Node *node;

  if (consume_keyword(TOKEN_RETURN) != NULL) {
    node = calloc(1, sizeof(Node));
    node->kind = NODE_RETURN;
    node->lhs = expression();
  }
  else if (consume_keyword(TOKEN_IF) != NULL) {
    expect("(");
    node = calloc(1, sizeof(Node));
    node->kind = NODE_IF;    
    node->lhs = expression();
    expect(")");
    node->rhs = statement();
    return node;
  }
  else {
    node = expression();
  }

  if (! consume(";")) {
    error_at(token->string, "Token other than ';' found here.");
  }
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

  while (true) {
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
    } else {
      return node;
    }
  }
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
  current_token->next = token;
  return token;
}

int is_token_character(char c) {
  return (isalpha(c) || isdigit(c) || c == '_');
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
      current_token = new_token(TOKEN_RESERVED, current_token, p);
      current_token->length = 2;
      p += 2;
      continue;
    }
    if (strncmp(p, "return", 6) == 0 && is_token_character(p[6]) == 0) {
      current_token = new_token(TOKEN_RETURN, current_token, p);
      current_token->length = 6;
      p += 6;
      continue;
    }
    if (strncmp(p, "if", 2) == 0 && is_token_character(p[2]) == 0) {
      current_token = new_token(TOKEN_IF, current_token, p);
      current_token->length = 2;
      p += 2;
      continue;
    }
    if (isalpha(*p) || *p == '_') {
      int name_length = 1 + strspn(p + 1, "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
      current_token = new_token(TOKEN_IDENTIFIER, current_token, p);
      current_token->length = name_length;
      p += name_length;
      continue;
    }
    if (!memcmp("+", p, 1) || !memcmp("-", p, 1) || !memcmp("*", p, 1) || !memcmp("/", p, 1) || !memcmp("(", p, 1) || !memcmp(")", p, 1)  || !memcmp("<", p, 1) || !memcmp(">", p, 1) || !memcmp("=", p, 1) || !memcmp(";", p, 1)) {
      current_token = new_token(TOKEN_RESERVED, current_token, p++);
      current_token->length = 1;
      continue;
    }
    if(isdigit(*p)) {
      current_token = new_token(TOKEN_NUMBER, current_token, p);
      char *current_pos = p;
      current_token->value = strtol(p, &p, 10);
      continue;
    }
    error_at(p, "cannot tokenize");
  }

  new_token(TOKEN_EOF, current_token, p);
  return head.next;
}
