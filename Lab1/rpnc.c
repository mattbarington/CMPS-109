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

Node* newNode(int data) {

}

//Node destructor
// void freeNode(Node* N) {
//     if( N!=NULL && *N!=NULL ){
//       free(*N);
//       *N = NULL;
//     }
// }


Node* push(Node* top, int data) {
    Node* newTop = (struct Node*) malloc(sizeof(Node));
    newTop -> next = top;
    newTop -> data = data;
    return newTop;
}

int pop(Stack* stack) {
    if (stack -> size < 1) {
      printf("Come on now, I ain't got no more to give.\n");
      return 0;
    }
    printf("%d is at the top of the stack\n", stack->top->data);
    Node* temp = stack -> top;
    stack -> top = stack -> top -> next;
    int result = temp -> data;
    //free previous top Node
    temp -> next = NULL;
    //free(&temp);
    stack -> size --;
    printf("%d is now at the top of the stack\n", stack->top->data);
    return result;
}

int main(int argc, char *argv[]) {
    Node* stack = n
    for (int i = 2; i < 10; i++) {
      printf("pushing %d to the stack \n", i);
      push(stack, i);
    }
    int ununsed; // here to fail warnings test, remove it
    printf("not implemented\n");

    return -1;

}
