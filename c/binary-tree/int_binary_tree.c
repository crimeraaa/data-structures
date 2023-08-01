#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "int_binary_tree.h"

treenode *create_node(int val)
{
    treenode *branch = malloc(sizeof(treenode));
    if (branch == NULL)
    {
        printf("Failed to allocate memory for node!\n");
        return NULL;
    }
    
    branch->val = val;
    branch->left = NULL;
    branch->right = NULL;
    return branch;
}

/**
 * @brief Given a value to search for, find its node or a node we can insert to.
 * 
 * @param root The pointer to the head of the binary tree.
 * @param val The value we want to find.
 * 
 * @return Node* 
 * 
 * @note This can be used in 2 ways: 
 * @note >> Check if the node already exists.
 * @note >> Find an empty spot with which we can insert to.
 */
static treenode *node_lookup(treenode *root, int val)
{
    /** 
     * @note Couple of possibilities for what may return:
     * @note 1.) <ptr> was <NULL> to begin with.
     * @note 2.) We found a node with an empty child pointer to insert to.
     * @note 3.) We found the exact value already in the tree.
    */
    treenode *ptr = root;

    /* Keep looking for a node with an empty child pointer to insert to. */
    while (ptr != NULL && val != ptr->val)
    {
        if (val < ptr->val)
        {
            if (ptr->left == NULL) break;
            else ptr = ptr->left;
        }
        else // if (val > ptr->val)
        {
            if (ptr->right == NULL) break;
            else ptr = ptr->right;
        }
    }
    return ptr;
}

bool insert_node(treenode **address_to_root, int val)
{
    treenode *ptr = *address_to_root;

    /** 
     * If the given root is empty, then fill it with values
     * Initially it should have all <NULL> child and parent pointers.
     * 
     * Do the assignment/value insertion via the call to <create_node>,
     * then immediately compare the result if it's NULL.
     * if <create_node> returns <NULL>, then we know we failed to alloc.
    */
    if (ptr == NULL) 
    {
        return ((*address_to_root) = create_node(val)) != NULL;
    }

    ptr = node_lookup(ptr, val);
    if (ptr == NULL) 
    {
        return false;
    }
    else if (val < ptr->val && ptr->left == NULL)
    {
        return (ptr->left = create_node(val)) != NULL;
    }
    else if (val > ptr->val && ptr->right == NULL)
    {
        return (ptr->right = create_node(val)) != NULL;
    }
    // else // if (val == ptr->val)
    return false;
}

bool search_tree(treenode *root, int val)
{
    treenode *ptr = node_lookup(root, val);
    if (ptr == NULL) 
    {
        return false;
    }
    return (ptr->val == val);
}

void free_tree_recurse(treenode *head)
{
    if (head == NULL)
    {
        return;
    }
    /**
     * I'm sure there's a way to do this non-recursively,
     * but I can't think of it!
     */
    free_tree_recurse(head->left);
    free_tree_recurse(head->right);

    free(head);
}