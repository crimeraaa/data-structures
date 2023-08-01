/**
 * @file genhashtable.c
 * @brief Implementation of Jacob Sorber's "Better Hashtable".
 * See: https://www.youtube.com/watch?v=KI_V91UdL1I
 * The aim of the video was to make a hashtable that can be applied
 * to any type of object, assuming the entire table contains just that object.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "genhashtable.h"

// Shorthand for "singly-linked list". Note the 2 lowercase 'L'.
struct sllist
{
    char *key;            // NUL terminated string to use as hash input.
    void *obj;            // obj ptr of any type, best to dynamically alloc.
    struct sllist *next;
};
// Shorthand for "singly-linked list node". Note the 2 lowercase 'L'.
typedef struct sllist sllnode;

// <hashtable> will update when <generic_hashtable> updates.
struct generic_hashtable 
{
    int size;                       // Total # of linked lists in the table.
    uint64_t collisions;            // Total # of collisions in the table.
    hash_function *hash_fn;         // Generates hash value from string key.
    cleanup_function *clean_fn;     // Custom object cleanup fn, or <free>.
    sllnode **elements;             // 1D array of linked lists for collisions.
};

/** 
 * @brief "private" function to properly call the hash_fn. 
 * @return size_t of the resulting hashed value.
 * @note  Check if <ht> and <key> are both not <NULL> before calling this.
*/
static size_t ht_hash(table *ht, const char *key)
{
    // Can return this directly, for debug see the <rax> register
    // or whatever register your device uses for return values.
    return (ht->hash_fn(key, strlen(key)) % ht->size);
}

table *ht_create(int size, hash_function *hf, cleanup_function *cf)
{
    // sizeof(table) is unknown at due to <table> being an opaque struct.
    // Use sizeof on the object that <ht> is pointing at for this case.
    table *ht = malloc(sizeof(*ht));
    if (ht == NULL) 
    {
        printf("Failed to allocate memory for hashtable!\n");
        return NULL;
    }
    ht->size = size;

    // calloc will 0 out the memory, useful for large list like this
    ht->elements = calloc(sizeof(sllnode*), ht->size);
    if (ht->elements == NULL)
    {
        free(ht);
        printf("Failed to allocate memory for hashtable's elements!\n");
        return NULL;        
    }
    ht->collisions = 0;
    ht->hash_fn = hf;
    
    // Ternary operator is useful for conditional variable assignment
    ht->clean_fn = (cf == NULL) ? free : cf;
    return ht;
}

void ht_destroy(table *ht)
{
    if (ht == NULL) return;

    // Get a handle to each linked individual linked list first.
    for (int i = 0; i < ht->size; i++)
    {
        // Traverse the linked list and free each element.
        sllnode *tmp = ht->elements[i];
        while (tmp != NULL)
        {
            sllnode *next = tmp->next;
            ht->clean_fn(tmp->obj);
            free(tmp->key);
            free(tmp);
            tmp = next;
        }
    }
    // ptr to linked lists and table itself were dynamically allocated also
    free(ht->elements);
    free(ht);
}

void ht_print(table *ht)
{
    if (ht == NULL) return;

    int empty_lists = 0;
    printf("---- START TABLE ----\n");
    for (int i = 0; i < ht->size; i++)
    {
        if (ht->elements[i] == NULL)
        {
            // Don't print <NULL> entries, not every compiler lets you do that
            empty_lists++;
            continue;
        }

        printf("%4i: ", i);
        sllnode *tmp = ht->elements[i];
        while (tmp != NULL)
        {
            // Due to objects being void*, no way to print them out reliably.
            printf("%s", tmp->key);
            tmp = tmp->next;

            // Print all elements of the linked list on the same line
            if (tmp != NULL) printf(", ");
        }
        // End of this linked list so print a newline for visual separation
        printf("\n");
    }
    printf("We have %zu total collisions and %i empty lists in the table.\n", 
        ht_collisions(ht), empty_lists);
    printf("---- END TABLE ----\n\n");
}

uint64_t ht_collisions(table *ht)
{
    //  Kind of stupid, but I would rather not segfault!
    if (ht == NULL) return 0;

    return ht->collisions;
}

bool ht_insert(table *ht, const char *key, void *obj)
{
    // Need to dereference <ht> and <key>, so check beforehand.
    if (ht == NULL || key == NULL || obj == NULL) return false;

    size_t idx = ht_hash(ht, key);

    // Don't reinsert an object with this exact key if it already exists.
    if (ht_find(ht, key) != NULL) return false;

    // Create new sllnode to be inserted.
    sllnode *tmp = malloc(sizeof(*tmp));
    if (tmp == NULL) return false;

    tmp->obj = obj;
    tmp->key = malloc((strlen(key) + 1) * sizeof(char));
    strcpy(tmp->key, key);

    // Update collision count if this index is occupied beforehand.
    if (ht->elements[idx] != NULL) ht->collisions++;

    // Usual linked list rearrangement
    tmp->next = ht->elements[idx];
    ht->elements[idx] = tmp;
    return true;
}

void *ht_find(table *ht, const char *key)
{
    if (ht == NULL || key == NULL) return NULL;

    size_t idx = ht_hash(ht, key);
    sllnode *tmp = ht->elements[idx];

    /*  Keep looking if the list is still valid and we haven't found the matching key */
    while (tmp != NULL && strcmp(tmp->key, key) != 0)
    {
        tmp = tmp->next;
    } 
    /*  Could not find the obj */
    if (tmp == NULL) return NULL;

    return tmp->obj;
}

bool ht_delete(table *ht, const char *key)
{
    if (ht == NULL || key == NULL) return false;

    size_t idx = ht_hash(ht, key);
    sllnode *tmp = ht->elements[idx];
    sllnode *prev = NULL;

    // Traverse list until we find the key or we reach the end of the list
    while (tmp != NULL && strcmp(tmp->key, key) != 0)
    {
        prev = tmp;
        tmp = tmp->next;
    }

    // Could not find the obj :(
    if (tmp == NULL) return false;
    
    // Deleting list head, so rearrange head of the list
    if (prev == NULL) 
        ht->elements[idx] = tmp->next;
    // Deleting from anywhere not list head, so rearrange middle of the list
    else 
        prev->next = tmp->next;

    // Main assumption: all of these were probably dynamically allocated
    ht->clean_fn(tmp->obj);
    free(tmp->key);
    free(tmp);
    return true;
}