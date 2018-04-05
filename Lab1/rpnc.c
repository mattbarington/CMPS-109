#include <stdio.h>
#include <stdlib.h>
/*
 * Reverse Polish Notation (RPN) Calculator
 *
 * Accepts ARGC arguments in RPN, evaluates them and prints the answer to stdout.
 *
 * Returns -1 on error, 0 otherwise.
 */

typedef struct Node {
    struct Node* next;
    int data;
} Node;


//Node destructor
// void freeNode(Node* N) {
//     if( N!=NULL && *N!=NULL ){
//       free(*N);
//       *N = NULL;
//     }
// }

void printStack(Node* this) {
    if (this != NULL) {
      printf("%d, ", this->data);
      printStack(this -> next);
    }
}

Node* push(Node* top, int data) {
    printf("pushing %d to the stack \n", data);
    Node* newTop = (struct Node*) malloc(sizeof(Node));
    newTop -> next = top;
    newTop -> data = data;
    return newTop;
}

Node* pop(Node* top) {
    if (top == NULL) {
      printf("Come on now, I ain't got no more to give.\n");
      return 0;
    }
    printf("%d is at the top of the stack\n", top->data);
    Node* temp = top;
    top = top -> next;
    //free previous top Node
    temp -> next = NULL;
    //free(&temp);
    printf("%d is now at the top of the stack\n", top->data);
    return top;
}

int main(int argc, char *argv[]) {
    Node* stack = NULL;
    for (int i = 2; i < 10; i++) {
      stack = push(stack, i);
      // printf("stack: ");
      // printStack(stack);
    }

    while (stack != NULL) {
      printf("%d \n", stack->data);
      stack = pop(stack);
    }

    int ununsed; // here to fail warnings test, remove it
    printf("not implemented\n");

    return -1;

}
