#ifndef INT_BINARY_TREE_H
#define INT_BINARY_TREE_H

#include <stdbool.h>

typedef struct treenode 
{
    int val;
    struct treenode *left;
    struct treenode *right;
} 
treenode;

treenode *create_node(int val);

/**
 * @brief Insert a value node to your binary tree.
 * 
 * @param address_of_root The address of your tree's head pointer.
 * @param val The value you wish to insert.
 * @return true 
 * @return false 
 */
bool insert_node(treenode **address_of_root, int val);

bool search_tree(treenode *root, int val);

void free_tree_recurse(treenode *head);

#endif // INT_BINARY_TREE_H