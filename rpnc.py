#
# Python3 Reverse Polish Notation (RPN) Calculator
#
# Accepts an arbitrary number of arguments in RPN, evaluates them and prints the answer to stdout.
#

def subtraction(stack):
    right, left = stack.pop(), stack.pop()
    stack.append(left - right)

def division(stack):
    right = stack.pop()
    left = stack.pop()
    stack.append(left / right)

def exponentiation(stack):
    right = stack.pop()
    left = stack.pop()
    stack.append(left ** right)

ops = {
    '*' : lambda stack : stack.append(stack.pop() * stack.pop()),
    '+' : lambda stack : stack.append(stack.pop() + stack.pop()),
    '-' : subtraction,
    '/' : division,
    '^' : exponentiation
    }

stack = []

from sys import stdin
eq = stdin.readline().split()
for token in eq:
    if token in ops:
        ops[token](stack)
    else:
        stack.append(int(token))

print(stack.pop())
