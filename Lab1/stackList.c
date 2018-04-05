#include <stdio.h>
#include <stdlib.h>

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
  printf("%d pushed to stack.\n", data);
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

int main(int argc, char *argv[]) {
  Stack* S = newStack();
  for (int i = 0; i < 4; i++) {
    push(S, i);
  }
  printf("Top of stack: %d\n", pop(S));
  printf("Top of stack: %d\n", pop(S));
  printf("Top of stack: %d\n", pop(S));
  printf("Top of stack: %d\n", pop(S));
  printf("Top of stack: %d\n", pop(S));

  free(S);
  return 0;
}
