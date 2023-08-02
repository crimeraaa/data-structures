#ifndef GENERAL_PURPOSE_BINARY_TREE_H
#define GENERAL_PURPOSE_BINARY_TREE_H

// I don't know why I'm doing this, I doubt I (or anybody else for that matter)
// would actually use these in their C project, much less a C++ project!
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus (start)

#include <stdbool.h>
#include <stdlib.h>

typedef struct bt_node
{
    void *obj;
    struct bt_node *parent;
    struct bt_node *lchild;
    struct bt_node *rchild;
}
bt_branch;

// Use these macros for your <cmp_obj> function.
#define IS_LCHILD  1 // Return value for comparefn, if parent >  child
#define IS_RCHILD -1 // Return value for comparefn, if parent <  child
#define BOTH_SAME  0 // Return value for comparefn, if parent == child

/**
 * @brief General comparison function typedef, useful for (almost) any datatype.
 * Make sure to cast the objects to the correct type inside the function body!
 * 
 * @param parent Must be of type (void*) for function declaration/definition.
 * @param child Must be of type (void*) for function declaration/definition.
 * 
 * @return 0 (parent == child), 1 (parent > child), -1 (parent < child)
 * 
 * @note This is under the assumption that both objects are of the same type!
 * @note This also assumes that both objects can be compared logically.
 * @note Will default to the '>', '<' and '==' and treat args as type (int).
 */
typedef int cmp_obj(void *parent, void *child);

/**
 * @brief Custom memory freeing functions in case your object has unique properties.
 * 
 * @param obj Must be of type (void*) for function declaration/definition.
 * 
 * @note This is under the assumption that the memory is dynamically allocated!
 * @note Will default to <free> from <stdlib.h> [ C ], or <cstdlib> [ C++ ]
 */
typedef void free_obj(void *obj);

/**
 * @brief Print out your object to <stdout>. Intended to be customizeable.
 * 
 * @param obj Must be of type (void*) for function declaration/definition.
 * 
 * @note Remember to cast to the correct type inside the function body!
 */
typedef void printobj(void *obj);


typedef struct bt_struct
{
    bt_branch *branch;
    cmp_obj   *comparefn;
    free_obj  *freefn;
    printobj  *printfn;
    size_t     nodecount;
}
bt_root;

/**
 * @brief Initialize your general-purpose binary tree.
 * The functions are intended to be customized for any datatype.
 * 
 * @param root Pass an existing bt_root variable's address to initialize.
 * @param cmpfn Pass <NULL> to compare 2 objects as if they are of type (int).
 * @param printfn Pass <NULL> to only print out the object's address.
 * @param freefn Pass <NULL> for <free> from <stdlib.h> [ C ], or <cstdlib> [ C++ ]
 * 
 * @return (bt_root*) a dynamically allocated handle to your binary tree.
 * 
 * @note <printfn>, <freefn> and <cmpfn> take params of type (void*)
 * @note The intention is for you to dereference them with your chosen type.
 * @note See "genbinarytree.h" for more information.
 */
bt_root *bt_init(cmp_obj *cmpfn, printobj *printfn, free_obj *freefn);

bool bt_insert(bt_root *root, void *obj);
bool bt_remove(bt_root *root, void *obj);
void *bt_search(bt_root *root, void *obj);
void bt_printbt(bt_root *root);
void bt_destroy(bt_root **root_address);

#ifdef __cplusplus
}
#endif // __cplusplus (end)

#endif // GENERAL_PURPOSE_BINARY_TREE_H