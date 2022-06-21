#ifndef _STACK_H
#define _STACK_H

/* type des élements contenus dans la pile */
typedef int elem;

typedef struct stack_base* stack;

void init_stack(stack*);
int is_empty(stack);
void push(stack*, elem);
elem pop(stack*);

#endif
