#include <yabtl.h>

// Copy a key of char * type.
void yabtl_copy_key_string
(
  yabtl_item **item,
  void *key
)
{
  ( *item )->key = ( void * )strdup( ( char * )key );
}

// Copy a key of int type.
void yabtl_copy_key_int
(
  yabtl_item **item,
  void *key
)
{
  ( *item )->key = malloc( sizeof( int ) );
  *( int * )( *item )->key = *( int * )key;
}

// Copy a key of uint32_t type.
void yabtl_copy_key_uint32_t
(
  yabtl_item **item,
  void *key
)
{
  ( *item )->key = malloc( sizeof( uint32_t ) );
  *( uint32_t * )( *item )->key = *( uint32_t * )key;
}

// Copy a key of uint8_t type.
void yabtl_copy_key_uint8_t
(
  yabtl_item **item,
  void *key
)
{
  ( *item )->key = malloc( sizeof( uint8_t ) );
  *( uint8_t * )( *item )->key = *( uint8_t * )key;
}

// Copy a key of char type.
void yabtl_copy_key_char
(
  yabtl_item **item,
  void *key
)
{
  ( *item )->key = malloc( sizeof( char ) );
  *( char * )( *item )->key = *( char * )key;
}

// Copy a key of unsigned char type.
void yabtl_copy_key_unsigned_char
(
  yabtl_item **item,
  void *key
)
{
  ( *item )->key = malloc( sizeof( unsigned char ) );
  *( unsigned char * )( *item )->key = *( unsigned char * )key;
}

// Copy a key of long type.
void yabtl_copy_key_long
(
  yabtl_item **item,
  void *key
)
{
  ( *item )->key = malloc( sizeof( long ) );
  *( long * )( *item )->key = *( long * )key;
}

// Allocate a node.
void yabtl_init_node
(
  yabtl *tree,
  yabtl_node **node
)
{
  int i;

  // Initialize the node, allocate memory for child and item pointers.
  ( *node )->leaf = true;
  ( *node )->count = 0;
  ( *node )->child = malloc( ( tree->order + 1 ) * sizeof( void * ) );
  ( *node )->item = malloc( tree->order * sizeof( yabtl_item * ) );

  // Initialize our child and item pointers.
  for ( i = 0; i < tree->order; i++ )
  {
    ( *node )->child[i] = NULL;
    ( *node )->item[i] = NULL;
  }
  ( *node )->child[i] = NULL;
}

// Set the comparison function pointer.
void yabtl_set_functions
(
  yabtl *tree
)
{
  switch ( tree->key_type )
  {
    case YABTL_STRING:
      tree->compare = &yabtl_compare_string;
      tree->copy_key = &yabtl_copy_key_string;
      break;
    case YABTL_INT:
      tree->compare = &yabtl_compare_int;
      tree->copy_key = &yabtl_copy_key_int;
      break;
    case YABTL_UINT32_T:
      tree->compare = &yabtl_compare_uint32_t;
      tree->copy_key = &yabtl_copy_key_uint32_t;
      break;
    case YABTL_UINT8_T:
      tree->compare = &yabtl_compare_uint8_t;
      tree->copy_key = &yabtl_copy_key_uint8_t;
      break;
    case YABTL_CHAR:
      tree->compare = &yabtl_compare_char;
      tree->copy_key = &yabtl_copy_key_char;
      break;
    case YABTL_UNSIGNED_CHAR:
      tree->compare = &yabtl_compare_unsigned_char;
      tree->copy_key = &yabtl_copy_key_char;
      break;
    case YABTL_LONG:
      tree->compare = &yabtl_compare_unsigned_char;
      tree->copy_key = &yabtl_copy_key_unsigned_char;
      break;
    default:
      tree->compare = &yabtl_compare_int;
      tree->copy_key = &yabtl_copy_key_int;
      break;
  }
}

// Initialize a b-tree.
void yabtl_init
(
  yabtl *tree,
  int order,
  yabtl_key_type key_type
)
{
  yabtl_node *node;

  // Set our items per node.
  tree->order = order;
  tree->key_type = key_type;
  yabtl_set_functions( tree );

  // Create our root node.
  node = malloc( sizeof( yabtl_node ) );
  yabtl_init_node( tree, &node );

  // Point the tree's root to this new node.
  tree->root = node;
}

// Binary search to find location of node.
int yabtl_binary_search
(
  yabtl *tree,
  yabtl_node *node,
  void *key
)
{
  int max;
  int min;
  int mid;
  yabtl_cmp cmp;
  void *mid_key;

  // See if it's empty first.
  if ( node->count == 0 )
  {
    return 0;
  }

  // Initalize range.
  min = 0;
  mid = 0;
  max = node->count - 1;

  // Find the insert position.
  while ( min <= max )
  {
    // Find the mid-point index.
    if ( min == max )
    {
      mid = min;
    } else
    {
      mid = ( ( min + max ) >> 1 );
    }

    // Get the corresponding key.
    mid_key = node->item[mid]->key;

    // Compare the two.
    cmp = tree->compare( mid_key, key );

    // Reduce our search range or return index.
    if ( cmp == LESS_THAN )
    {
      min = mid + 1;
    } else if ( cmp == GREATER_THAN )
    {
      max = mid - 1;
    } else
    {
      return mid;
    }
  }

  // Return the closest index in negative form.
  return -( min + 1 );
}

// Insert an item into a node.
void yabtl_insert_item
(
  yabtl *tree,
  yabtl_node **node,
  int index,
  yabtl_item *item
)
{
  int i;

  // Move over existing items.
  for ( i = ( *node )->count; i > index; i-- )
  {
    ( *node )->item[i] = ( *node )->item[i - 1];
  }

  // Increment count.
  ( *node )->count++;

  // Update the pointer.
  ( *node )->item[index] = item;
}

// Split a node into two.
yabtl_item *yabtl_split_node
(
  yabtl *tree,
  yabtl_node **left,
  yabtl_node **right
)
{
  int i;
  int mid;
  yabtl_item *mid_item;

  // Set the leaf flag.
  ( *right )->leaf = ( *left )->leaf;

  // Find the midpoint.
  mid = ( tree->order >> 1 );
  mid_item = ( *left )->item[mid];

  // Move half of the items and children to the right node.
  for ( i = mid + 1; i < tree->order; i++ )
  {
    ( *right )->item[i - mid - 1] = ( *left )->item[i];
    ( *left )->item[i] = NULL;
    ( *right )->child[i - mid - 1] = ( *left )->child[i];
    ( *left )->child[i] = NULL;
  }
  ( *right )->child[i - mid - 1] = ( *left )->child[i];
  ( *left )->child[i] = NULL;

  // Update counts.
  ( *right )->count = ( *left )->count - mid - 1;
  ( *left )->count = mid;

  // Make the mid item null in the left node.
  ( *left )->item[mid] = NULL;

  // Return our mid item so it can be inserted into the parent.
  return mid_item;
}

// Insert an item into the tree.
yabtl_item *yabtl_insert_recursive
(
  yabtl *tree,
  yabtl_node **node,
  void *key,
  void *data,
  bool *needs_split
)
{
  int index;
  int i;
  yabtl_item *result;
  yabtl_node *new_node;
  bool found;
  yabtl_item *to_move;

  // Make sure we weren't passed a null node.
  if ( node == NULL || *node == NULL )
  {
    return NULL;
  }

  // Try to find our insert position.
  index = 0;
  found = false;
  if ( ( *node )->count == 0 )
  {
    index = 0;
  } else
  {
    index = yabtl_binary_search( tree, *node, key );
    if ( index >= 0 )
    {
      found = true;
    } else
    {
      index = index * -1 - 1;
    }
  }

  if ( found == true )
  {
    // Obviously don't need to split this node.
    *needs_split = false;

    // If we found the key in this node, return it without updating the data.
    return ( *node )->item[index];
  }

  if ( ( *node )->leaf == true )
  {
    // Insert the item in this leaf.
    result = malloc( sizeof( yabtl_item ) );
    tree->copy_key( &result, key );
    result->data = data;

    // If the node isn't empty, insert it directly.
    if ( ( *node )->count < tree->order - 1 )
    {
      *needs_split = false;
    } else
    {
      *needs_split = true;
    }

    // Insert the item.
    yabtl_insert_item( tree, node, index, result );
    return result;
  }

  // If it's not a leaf, go to the child at that index.
  result = yabtl_insert_recursive( tree, ( yabtl_node **)&( *node )->child[index], key, data, needs_split );
  if ( *needs_split == false )
  {
    return result;
  }

  // We have to split this child node.
  new_node = malloc( sizeof( yabtl_node ) );
  yabtl_init_node( tree, &new_node );
  to_move = yabtl_split_node( tree, ( yabtl_node ** )&( *node )->child[index], &new_node );

  if ( ( *node )->count < tree->order - 1 )
  {
    *needs_split = false;
  } else
  {
    *needs_split = true;
  }
  yabtl_insert_item( tree, node, index, to_move );

  // Move over children at index + 1 to make room for new child.
  for ( i = tree->order; i > index + 1; i-- )
  {
    ( *node )->child[i] = ( *node )->child[i - 1];
  }
  ( *node )->child[index + 1] = new_node;
  return result;
}

// Wrapper for recursive insert function.
yabtl_item *yabtl_insert
(
  yabtl *tree,
  void *key,
  void *data
)
{
  yabtl_item *result;
  yabtl_node *old_root;
  yabtl_node *new_node;
  yabtl_item *to_move;
  bool needs_split;

  needs_split = false;
  result = yabtl_insert_recursive( tree, &tree->root, key, data, &needs_split );
  if ( needs_split == true )
  {
    // Split the current root node.
    new_node = malloc( sizeof( yabtl_node ) );
    yabtl_init_node( tree, &new_node );
    to_move = yabtl_split_node( tree, &tree->root, &new_node );

    // Create the new root.
    old_root = tree->root;
    tree->root = malloc( sizeof( yabtl_node ) );
    yabtl_init_node( tree, &tree->root );

    // Insert the new middle item and create child pointers.
    yabtl_insert_item( tree, &tree->root, 0, to_move );
    tree->root->child[0] = old_root;
    tree->root->child[1] = new_node;

    // Update the leaf flag.
    tree->root->leaf = false;
  }

  // Return new item.
  return result;
}
