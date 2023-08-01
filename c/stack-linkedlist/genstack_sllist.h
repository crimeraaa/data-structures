#ifndef GENERAL_PURPOSE_STACK_SLLIST_H
#define GENERAL_PURPOSE_STACK_SLLIST_H

#ifdef __cplusplus
extern "C" {
#endif // starting { of extern "C"

#include <stdbool.h>

/** 
 * @brief Specify how you'd like to format your objects' printouts. 
 * @note  Example, pass a wrapper function that contains:
 * @note  printf("%s\n", (char*)ptr->obj) 
*/
typedef void objprint_fn(void *obj);

/** 
 * @brief Similar to objprint, specify how you'd like to free your objects.
 * @note  Pass <NULL> to default to free.
 * @note  This assumes your objects are dynamically allocated.
 * @note  Otherwise, pass a function that does nothing.
*/
typedef void objfree_fn(void *obj);

// shorthand for "singly linked list"
typedef struct sllist
{
    void *obj;
    struct sllist *next;
}
// shorthand for "singly linked list node"
sllnode;

typedef struct stackobj
{
    sllnode *list;          // Our stack proper is just a singly linked list.
    objfree_fn *free_obj;
    objprint_fn *print_obj;
} 
Stack;

/**
 * @brief           Initialise a general purpose stack.
 * @param obj       Initial object to start the stack off with, or <NULL>
 * @param print_fn  Pass <NULL> to print out only the object's address. 
 * @param free_fn   Pass <NULL> to use <free> from stdlib.
 * @note            Be careful if you pass <NULL> for <obj> though!
 */
Stack *stack_init(void *obj, objprint_fn *print_obj, objfree_fn *free_obj);

bool stack_push(Stack *stack, void *obj);

/** 
 * @brief Frees the topmost element of the stack.
 * @return (void*) to the object from the freed element. 
 * @note The object itself is not freed.
*/
void *stack_pop(Stack *stack);

void stack_print(Stack *stack);

void stack_destroy(Stack *stack);

#ifdef __cplusplus
}
#endif // closing } of extern "C"

#endif // GENERAL_PURPOSE_STACK_SLLIST_H