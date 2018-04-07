#include <stdio.h>
#include <stdlib.h> //malloc
#include <string.h> //strlen, strtok

/**
 * Stack implentation inspired by information from
 * https://www.geeksforgeeks.org/stack-data-structure-introduction-program/
 */

typedef struct Node {
  int data;
  struct Node* next;
} Node;

typedef struct Stack {
  Node* top;
} Stack;

Node* newNode(int data) {
  Node* n = (Node*) malloc(sizeof(Node));
  n->data = data;
  n->next = NULL;
  return n;
}

Stack* newStack() {
  Stack* S = (Stack*) malloc(sizeof(Stack));
  S->top = NULL;
  return S;
}

void push(Stack* S, int data) {
  Node* n = newNode(data);
  n->next = S->top;
  S->top = n;
}

int pop(Stack* S) {
  if (S == NULL) {printf("Error in pop: NULL Stack reference\n"); return 0;}
  else if(S-> top == NULL) {printf("Cannot pop from an empty Stack\n"); return 0;}
  int temp = S->top->data;
  Node* n = S->top;
  S->top = n->next;
  n->next = NULL;
  free(n);
  return temp;
}

int isEmpty(Stack* S) {
  if (S == NULL) {printf("Error in isEmpty: NULL Stack reference\n"); return 1;}
  return !S->top;
}

int isOperator(char *c) {
  return !(strcmp(c,"+") && strcmp(c,"-") && strcmp(c,"*") &&
  strcmp(c,"/") && strcmp(c,"^"));

}

int expon(int a, int b) {
  int result = a;
  for (int i = 1; i < b; i++) {
    result = a * result;
  }
  return result;
}

/**
 * String tokenizing strtok referenced from
 * https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
 */
int main(int argc, char *argv[]) {
  Stack* S = newStack();
  //used to iterate throuch tokens in string
  char *token = strtok(argv[1], " ");
  while (token != NULL) {
    if (isOperator(token)) {
      int right_oper = pop(S);
      int left_oper = pop(S);
      // decide which operator is being used, and use it
      if (!strcmp(token, "+")) push(S, left_oper + right_oper);
      else if (!strcmp(token, "-")) push(S, left_oper - right_oper);
      else if (!strcmp(token, "*")) push(S, left_oper * right_oper);
      else if (!strcmp(token, "/")) push(S, left_oper / right_oper);
      else if (!strcmp(token, "^")) push(S, (int) expon(left_oper, right_oper));
    } else {
      push(S, atoi(token));
    }
    token = strtok(NULL, " ");
  }
  printf("%d", pop(S));


  // while (!isEmpty(S)) {
  //   pop(S);
  // }
  free(S);
  return 0;
}
