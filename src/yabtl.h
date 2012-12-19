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
  YABTL_UNSIGNED_STRING,
  YABTL_INT,
  YABTL_UINT32_T,
  YABTL_UINT8_T,
  YABTL_CHAR,
  YABTL_UNSIGNED_CHAR,
  YABTL_LONG,
} yabtl_key_type;

// Structure of a single item.
typedef struct
{
  void *key;  // Pointer to key for this item.
  void *data; // Pointer to data for this item.
} yabtl_item;

// Definition of a single b-tree node.
typedef struct
{
  uint32_t count;    // Number of items in this node.
  yabtl_item **item; // Items within this node.
  void **child;      // Pointers to the n+1 children of this node.
  bool leaf;         // Flag indicating whether or not this node is a leaf.
} yabtl_node;

// Definition of a b-tree.
typedef struct
{
  yabtl_node *root;        // Pointer to root node.
  int order;               // The number of items per node for this b-tree.
  yabtl_key_type key_type; // Type of data for the key.
  yabtl_cmp ( *compare )( void *, void * ); // Pointer to comparision function.
  void ( *copy_key )( yabtl_item **, void * ); // Pointer to the key copy function.
} yabtl;

// Iterator stack.
struct yabtl_iterator_stack
{
  struct yabtl_iterator_stack *next;
  yabtl_node *node;
  int index;
  bool child_next;
};

// Definition of a b-tree iterator.
typedef struct
{
  struct yabtl_iterator_stack *stack;
} yabtl_iterator;

// Functions.
extern yabtl_cmp yabtl_compare_string( void *, void * );
extern yabtl_cmp yabtl_compare_unsigned_string( void *, void * );
extern yabtl_cmp yabtl_compare_int( void *, void *key_2 );
extern yabtl_cmp yabtl_compare_uint32_t( void *, void * );
extern yabtl_cmp yabtl_compare_uint8_t( void *, void * );
extern yabtl_cmp yabtl_compare_char( void *, void * );
extern yabtl_cmp yabtl_compare_unsigned_char( void *, void * );
extern yabtl_cmp yabtl_compare_long( void *, void * );
extern yabtl_node *yabtl_allocate_node( yabtl * );
extern void yabtl_init( yabtl *, int, yabtl_key_type );
extern void yabtl_init_node( yabtl *, yabtl_node ** );
extern bool yabtl_split_child( yabtl *, yabtl_node *, uint32_t );
extern yabtl_item *yabtl_insert( yabtl *, void *, void * );
extern void yabtl_insert_item( yabtl *, yabtl_node **, int, yabtl_item * );
extern yabtl_cmp ( *yabtl_compare )( void *, void * );
extern yabtl_item *yabtl_search( yabtl *, void *key );
extern void yabtl_destroy_node( yabtl_node ** );
extern yabtl_item *yabtl_iterate( yabtl_iterator * );
extern void yabtl_init_iterator( yabtl *, yabtl_iterator * );
extern void yabtl_iterate_recursive( yabtl *, yabtl_node * );
extern int yabtl_binary_search(yabtl *, yabtl_node *, void *);
extern void yabtl_destroy( yabtl * );
extern void yabtl_delete( yabtl *, void * );
