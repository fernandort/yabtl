#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Return values for comparison functions.
typedef enum yabtl_cmp {
  LESS_THAN,
  EQUAL_TO,
  GREATER_THAN
} yabtl_cmp;

// Data types for the keys.
typedef enum yabtl_key_type
{
  YABTL_STRING,
  YABTL_INT,
  YABTL_UINT32_T,
  YABTL_UINT8_t,
  YABTL_CHAR,
  YABTL_UNSIGNED_CHAR,
  YABTL_LONG,
} yabtl_key_type;

// Structure of a single item.
typedef struct
{
  void *key;  // Pointer to key for this item.
  void *data; // Pointer to data for this item.
  uint32_t i; // Index of this item within the node.
} yabtl_item;

// Definition of a single b-tree node.
typedef struct
{
  uint32_t count;   // Number of items in this node.
  yabtl_item *item; // Items within this node.
  void **child;     // Pointers to the n+1 children of this node.
  bool leaf;        // Flag indicating whether or not this node is a leaf.
} yabtl_node;

// Definition of a b-tree.
typedef struct
{
  yabtl_node *root;        // Pointer to root node.
  uint32_t items_per_node; // The number of items per node for this b-tree.
  yabtl_key_type key_type; // Type of data for the key.
} yabtl;
