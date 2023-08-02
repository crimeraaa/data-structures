#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "genstack_sllist.h"

static void default_objprint(void *obj)
{
    printf("%p\n", obj);
}

Stack *stack_init(void *obj, objprint_fn *print_obj, objfree_fn *free_obj)
{
    Stack *stack = malloc(sizeof(Stack));
    if (!stack) return NULL;

    // The very first element isn't allocated yet, so do it here
    stack->list = malloc(sizeof(sllnode));
    if (!stack->list) 
    {
        free(stack);
        return NULL;
    }

    stack->list->obj  = obj;
    stack->list->next = NULL;

    stack->print_obj = (print_obj) ? print_obj : default_objprint;
    stack->free_obj  = (free_obj)  ? free_obj  : free;
    return stack;
}

bool stack_push(Stack *stack, void *obj)
{
    sllnode *top = malloc(sizeof(sllnode));
    if (!top) return false;

    top->obj = obj;

    // Keep the original ptr in memory before rearranging the list
    top->next = stack->list;

    // Put our newly created sllnode to the very top of the stack
    stack->list = top;
    return true;
}

void *stack_pop(Stack *stack)
{
    if (!stack || !stack->list) return NULL;

    sllnode *top = stack->list;

    // Remove/free topmost entry from the stack
    // return the object stored to user, if malloc'd they can free it
    void *obj = stack->list->obj;
    stack->list = stack->list->next;
    free(top);
    return obj;
}

void stack_print(Stack *stack)
{
    if (!stack) return;

    size_t i = 1;
    printf("************************\n");
    printf("       << START >>\n");
    sllnode *ptr = stack->list;
    while (ptr)
    {
        printf("%zu.) ", i);
        stack->print_obj(ptr->obj);
        ptr = ptr->next;
        i++;
    }
    printf("        << END >>       \n");
    printf("************************\n");
}

void stack_destroy(Stack **ptr_address)
{
    Stack *stack = *ptr_address;
    if (!stack) return;

    sllnode *ptr = stack->list;
    while (ptr)
    {
        sllnode *tmp = ptr;
        ptr = ptr->next;
        stack->free_obj(tmp->obj);
        free(tmp);
    }
    free(stack);
    *ptr_address = NULL;
}