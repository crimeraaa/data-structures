/**
 * @file genhashtable.h
 * @brief Implementation of Jacob Sorber's "Better Hashtable".
 * See: https://www.youtube.com/watch?v=KI_V91UdL1I
 * The aim of the video was to make a hashtable that can be applied
 * to any type of object, assuming the entire table contains just that object.
 */

#ifndef GENERIC_HASHTABLE_H
#define GENERIC_HASHTABLE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Actual implementation of the hash function is up to you.
 * @brief It's meant to be a member of the hashtable struct.
 * @param key the NUL terminated string you want to use as the key.
 * @param length the string's length. Use strlen or similar.
 * @return uint64_t for your hashed value.
 * @note The parameters must always be the same.
  */
typedef uint64_t hash_function(const char *key, size_t length);

/**
 * @brief The function to be called to free your object.
 * @brief It's meant to be a member of the hashtable struct.
 * @param obj pointer to the object to be freed, may be any datatype.
 * @note Pass NULL for cf in ht_create to default to free().
 * @note The parameters must always be the same.
  */
typedef void cleanup_function(void *obj);

/**
 * @brief A generic hashtable.
 * @note Forward declared in genhashtable.h as an opaque struct.
 * @note Actual implementation is in the genhashtable.c file.
  */
typedef struct generic_hashtable table;
    
/**
 * @brief Initialize your table and return a handle to it.
 * @param size How many indexes/linked lists you want in the table.
 * @param hf Custom hash function. See hash_function in genhashtable.h.
 * @param cf Custom cleanup function for your object type. Pass NULL for free().
 * @return A pointer to your table on success, NULL otherwise.
 * @note This will malloc space for your table. Remember to free it later on.
 * @note cf can be specific to cleanup custom datatypes.
 */
table *ht_create(int size, hash_function *hf, cleanup_function *cf);

/**
 * @brief Frees your entire table and each of its linked lists.
 * @param ht The pointer to the hashtable you wish to free.
 * @note This assumes each entry has a malloc'd key and malloc'd obj.
  */
void ht_destroy(table *ht);

/**
 * @brief Loops through entire table and prints each linked list element.
 * @param ht The pointer to the hashtable you want to print out.
 * @note The prints are ordered in terms of the hashed keys.
 * @note Linked list is ordered from most recent to oldest elements.
  */
void ht_print(table *ht);

/**
 * @brief Dereferences ht and returns its number of collisions.
 * @param ht The pointer to the hashtable you want to check.
 * @return uint64_t of #collisions, else 0 if <ht> is <NULL>.
  */
uint64_t ht_collisions(table *ht);

/**
 * @brief Insert obj into the hashtable using the given key.
 * @param ht Pointer to the hashtable where the object will be inserted.
 * @param key Object's NUL termianted string key to be hashed.
 * @param obj A void pointer, can point to any datatype.
 * @return true if successful, false otherwise.
 * @note Best to malloc/calloc space for your object.
  */
bool ht_insert(table *ht, const char *key, void *obj);

/**
 * @brief Tries to find an object in the hashtable using the given key.
 * @param ht Pointer to hashtable in which the element should be located.
 * @param key Object's NUL terminated string key to be hashed.
 * @return Pointer to the object. Otherwise, NULL if not found in the list.
  */
void *ht_find(table *ht, const char *key);

/**
 * @brief Frees memory for a specific element and its members.
 * @param ht Pointer to hashtable in which the element should be located.
 * @param key Object's NUL terminated string key to be hashed.
 * @return true if successful, false otherwise.
  */
bool ht_delete(table *ht, const char *key);

#endif // GENERIC_HASHTABLE_H