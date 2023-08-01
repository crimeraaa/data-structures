#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "genbinarytree.h"

#define OBJ_EXISTS 1
#define OBJ_NOSPOT 2

// "Private" functions not meant for you, the user, to invoke in your program.
// These are just helper functions or default functions specific to this file.

static bt_branch *bt_lookup(bt_root *root, void *obj);
static int bt_cmpfn(void *parent, void *child);
static void bt_destroy_recurse(free_obj *freefn, bt_branch *node);
static void bt_printfn(void *obj);
static void bt_print_recurse(int recurse, printobj *objprintfn, bt_branch *node);
static void bt_errorprint(int errcode, printobj *printfn, bt_branch *parent, void *obj);
static void reassign_children(int cmp, bt_branch *parent, bt_branch *target);

bt_root *bt_init(cmp_obj *cmpfn, printobj *printfn, free_obj *freefn)
{
    bt_root *new_bt = malloc(sizeof(bt_root));
    if (new_bt == NULL) return NULL;

    // Start branch off with <NULL> else we get garbage.
    new_bt->branch    = NULL;
    new_bt->comparefn = (cmpfn)   ? cmpfn   : bt_cmpfn;
    new_bt->printfn   = (printfn) ? printfn : bt_printfn;
    new_bt->freefn    = (freefn)  ? freefn  : free;
    new_bt->nodecount = 0;

    return new_bt;
}

bool bt_insert(bt_root *root, void *obj)
{
    if (root == NULL) return false;
    
    bt_branch *node = malloc(sizeof(bt_branch));

    if (node == NULL) return false;

    // Initialize members to NULL so we don't keep garbage values around.
    node->obj    = obj;
    node->parent = NULL;
    node->lchild = NULL;
    node->rchild = NULL;

    bt_branch *target = root->branch;

    if (target == NULL)
    {
        // If list is empty, <node> is set to be the very first element.
        root->nodecount++;
        root->branch = node;
        return true;
    }

    // bt_lookup has no conditions that return <NULL>.
    // At least that's my hope!
    target = bt_lookup(root, obj);

    int cmp  = root->comparefn(target->obj, obj);

    bool found_spot = false;

    switch (cmp)
    {
        case IS_LCHILD: if (target->lchild == NULL) 
                        {
                            found_spot      = true;
                            node->parent    = target;
                            target->lchild  = node;
                        }
                        // Mainly a debugging information for when I mess up.
                        else 
                        {
                            bt_errorprint(OBJ_NOSPOT,  root->printfn, target, obj);
                            found_spot = false;
                        }
                        break;

        case IS_RCHILD: if (target->rchild == NULL) 
                        {
                            found_spot     = true;
                            node->parent   = target;
                            target->rchild = node;
                        }
                        // Same explanation as the "IS_RCHILD" case.
                        else 
                        {
                            bt_errorprint(OBJ_NOSPOT, root->printfn, target, obj);
                            found_spot = false;
                        }
                        break;

        // Don't insert since it already exists
        case BOTH_SAME: bt_errorprint(OBJ_EXISTS, root->printfn, target, obj);
                        found_spot = false;
                        break;

    }
    if (found_spot == true)
        root->nodecount++;
    else 
        free(node);

    return found_spot;
}

static void bt_errorprint(int errcode, printobj *printfn, bt_branch *parent, void *obj)
{
    switch (errcode)
    {
        case OBJ_EXISTS: printf("[ERROR: %i] - Object already exists in the tree!\n", errcode);
                         break;

        case OBJ_NOSPOT: printf("[ERROR: %i] - Could not find a spot to insert to!", errcode);
                         break;

        default:         printf("[UNKNOWN ERROR: %i]\n", errcode);
                         break;
    }

    printf("Tried to insert:\n"); 
    printf("           obj = "); printfn(obj);
    printf("Parent node would've been:\n");
    printf("   parent->obj = "); printfn(parent->obj);
    printf("\n");
}

bool bt_remove(bt_root *root, void *obj)
{
    bt_branch *target = bt_lookup(root, obj);

    // Tree itself is invalid or the object does not exist in the tree
    if (target == NULL || root->comparefn(target->obj, obj) != BOTH_SAME)
        return false;

    // Reassign the children's parent pointers so we don't lose their handles.
    if (!target->parent)
    {
        // Maybe one day I'll figure this out, but for now too lazy
        printf("[ERROR] Cannot delete the top of the binary tree yet!\n");
        return false;
    }

    int cmp = root->comparefn(target->parent->obj, target->obj);

    reassign_children(cmp, target->parent, target);
    
    root->freefn(target->obj);
    free(target);
    return true;
}

/**
 * @note This may leak memory, have fun :)
 * @todo Properly reassign if target's children nodes also have children.
 */
static void reassign_children(int cmp, bt_branch *parent, bt_branch *target)
{
    // shorthand meaning "target node's left child".
    bt_branch *tlchild = target->lchild;

    // shorthand meaning "target node's right child".
    bt_branch *trchild = target->rchild;

    bool both_null = !tlchild && !trchild;
    bool both_true =  tlchild &&  trchild;

    // Clear the cases where both children return the same boolean.
    // That is, target node either as no children or it has 2 children.
    if (both_null || both_true)
    {
        switch (cmp) 
        {
            // If we're removing a node with 2 children,
            // target's right/left child will "adopt" the other one.
            case IS_LCHILD: parent->lchild = (trchild) ? trchild : NULL;
                            if (trchild) 
                                trchild->lchild = tlchild;
                            break;

            case IS_RCHILD: parent->rchild = (tlchild) ? tlchild : NULL;
                            if (tlchild)
                                tlchild->rchild = trchild;
                            break;
        }
        // Return so we don't need to make an explicit else case with indent.
        return;
    }

    // Assumption here is that one child isn't NULL but the other is.
    // Set parent->left/right child to whichever target->child isn't NULL.
    switch (cmp)
    {
        // If this doesn't make sense, try drawing it out on paper!
        case IS_LCHILD: parent->lchild = (tlchild) ? tlchild : trchild;
                        break;

        case IS_RCHILD: parent->rchild = (trchild) ? trchild : tlchild;
                        break;
    }
}

void *bt_search(bt_root *root, void *obj)
{
    bt_branch *ptr = bt_lookup(root, obj);

    // Object does not exist in the list or the list itself is invalid
    if (!ptr || root->comparefn(ptr, obj) != BOTH_SAME)
        return NULL;
    return ptr->obj;
}

void bt_printbt(bt_root *root)
{
    int recurse = 0;
    bt_print_recurse(recurse, root->printfn, root->branch);
}

static void bt_print_recurse(int recurse, printobj *printfn, bt_branch *node)
{
    // Base case for recursion to avoid infinite loops.
    // I'm having a hard time finding a non-recursive way of going about this.
    if (!node) return;

    for (int i = 0; i < recurse; i++)
        printf("\t");

    printfn(node->obj);

    // Don't use recurse++, we want the same levels to have the same indent.
    bt_print_recurse(recurse + 1, printfn, node->lchild);
    bt_print_recurse(recurse + 1, printfn, node->rchild);
}

void bt_destroy(bt_root *root)
{
    if (!root) return;

    bt_destroy_recurse(root->freefn, root->branch);
    free(root);
}

static void bt_destroy_recurse(free_obj *freefn, bt_branch *node)
{
    // Base case for recursion to avoid infinite loops.
    // I'm having a hard time finding a non-recursive way of going about this.
    if (!node) return;

    // Free the binary tree from bottom-up in a "walkdown-walkup" motion.
    bt_destroy_recurse(freefn, node->lchild);
    bt_destroy_recurse(freefn, node->rchild);

    freefn(node->obj);
    free(node);
}

/**
 * @brief Look for the given object if it already exists in the binary tree.
 * 
 * @return (bt_branch*) to empty spot to insert, or matching (bt_branch*).
 * 
 * @note Used as both a way to insert and a way to search the list.
 */
static bt_branch *bt_lookup(bt_root *root, void *obj)
{
    if (!root) return NULL;

    bt_branch *node = root->branch;
    int cmp;
    
    while (node)
    {
        cmp = root->comparefn(node->obj, obj);
        
        // Using <goto> to break out of both <while> and <switch> scopes.
        switch (cmp)
        {
            case IS_LCHILD: if (!node->lchild) 
                                goto end_lookup;
                            else 
                                node = node->lchild;
                            break;

            case IS_RCHILD: if (!node->rchild) 
                                goto end_lookup;
                            else 
                                node = node->rchild;
                            break;
                            
            case BOTH_SAME: goto end_lookup;
        }
    }
    end_lookup:
    return node;
}

/**
 * @brief Default object-compare function.
 * @brief Compares 2 (void*) objects as if they were (int*).
 * 
 * @return 0 (parent == child), 1 (parent > child), -1 (parent < child)
 */
static int bt_cmpfn(void *parent, void *child)
{
    int x = *(int*)parent;
    int y = *(int*)child;

    if (x > y) 
        return IS_LCHILD;
    else if (x < y) 
        return IS_RCHILD;
    // Implied else, x == y
    return BOTH_SAME;
}

/**
 * @brief Default object-print function.
 * Prints only the address of the object pointer and nothing else.
 */
static void bt_printfn(void *obj)
{
    printf("%p\n", obj);
}