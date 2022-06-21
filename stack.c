#include "stack.h"
#include <stdlib.h>
struct stack_base{
    elem val;
    struct stack_base* next;
};
void init_stack(stack* s){
   *s=NULL; 
}

int is_empty(stack s){
    return s==NULL;
}
stack cons(elem v, stack n){
    stack r;
    r=malloc(sizeof(struct stack_base));
    r->val=v;
    r->next=n;
    return r;
}
void push(stack *ps, elem c){
    (*ps)=cons(c, (*ps));
}
elem pop(stack *s){
    elem c=(*s)->val;
    stack tmp= *s;
    *s=(*s)->next;
    free(tmp);
    return c;
}
