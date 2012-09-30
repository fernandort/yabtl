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
  ( *node )->child = malloc( tree->order * sizeof( void * ) );
  ( *node )->item = malloc( tree->order * sizeof( yabtl_item * ) );

  // Initialize our child and item pointers.
  for ( i = 0; i < tree->order; i++ )
  {
    ( *node )->child[i] = NULL;
    ( *node )->item[i] = NULL;
  }
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
  uint32_t order,
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

  // Insert directly, no need to split.
  for ( i = ( *node )->count; i > index; i-- )
  {
    ( *node )->item[i] = ( *node )->item[i - 1];
  }
  ( *node )->count++;

  // Update the pointer.
  ( *node )->item[index] = item;
}

// Split a node into two.
yabtl_item *yabtl_split_node
(
  yabtl *tree,
  yabtl_node **left,
  yabtl_node **right,
  yabtl_item *item,
  int index
)
{
  int i;
  int mid;
  yabtl_item *mid_item;

  // Insert it into the left node first (making the count too high).
  yabtl_insert_item( tree, left, index, item );

  // Set the leaf flag.
  ( *right )->leaf = ( *left )->leaf;

  // Find the midpoint.
  mid = ( ( *left )->count >> 1 );
  mid_item = ( *left )->item[mid];

  // Move half of the items and children to the right node.
  for ( i = mid + 1; i < ( *left )->count; i++ )
  {
    ( *right )->item[i - mid - 1] = ( *left )->item[i];
    ( *right )->child[i - mid - 1] = ( *left )->child[i];
    ( *left )->item[i] = NULL;
    ( *left )->child[i] = NULL;
  }

  // Update counts.
  ( *right )->count = ( *left )->count - mid - 1;
  ( *left )->count = mid;

  // Make the mid item null in the left node.
  ( *left )->item[mid] = NULL;

  // Return our mid item so it can be inserted into the parent.
  return mid_item;
}

// Insert an item into this node.
yabtl_item *yabtl_insert_item_in_node
(
  yabtl *tree,
  yabtl_node **node,
  yabtl_item **to_be_inserted,
  yabtl_node **left_child,
  yabtl_node **right_child,
  yabtl_item *new_item,
  int index,
  bool update_children
)
{
  yabtl_node *new_node;

  // If the node isn't empty, insert it directly.
  if ( ( *node )->count < tree->order - 1 )
  {
    // Insert directly, no need to split.
    yabtl_insert_item( tree, node, index, new_item );

    // Update child pointers.
    if ( update_children == true )
    {
      ( *node )->child[index] = *left_child;
      ( *node )->child[index + 1] = *right_child;
    }

    // We don't have a node to insert into the parent node.
    *to_be_inserted = NULL;
    *left_child = NULL;
    *right_child = NULL;

    // Return this item.
    return new_item;
  }

  // If we got here, we need to split this node.
  new_node = malloc( sizeof( *new_node ) );
  yabtl_init_node( tree, &new_node );

  // Split the node and insert the new item.
  *to_be_inserted = yabtl_split_node( tree, node, &new_node, new_item, index );
  *left_child = *node;
  *right_child = new_node;
  //printf( "Split, moving up %d\n", *( int * )( *to_be_inserted )->key );

  // Return the new item.
  return new_item;
}

// Insert an item into the tree.
yabtl_item *yabtl_insert_recursive
(
  yabtl *tree,
  yabtl_node **node,
  yabtl_item **to_be_inserted,
  yabtl_node **left_child,
  yabtl_node **right_child,
  void *key,
  void *data
)
{
  int index;
  int i, j, mid;
  yabtl_item *result;
  yabtl_node *new_node;
  void *temp;
  bool found;

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
    // If we found the key in this node, return it without updating the data.
    return ( *node )->item[index];
  }

  if ( ( *node )->leaf == true )
  {
    // Insert the item in this leaf.
    result = malloc( sizeof( yabtl_item ) );
    tree->copy_key( &result, key );
    result->data = data;
    return yabtl_insert_item_in_node( tree, node, to_be_inserted, left_child, right_child, result, index, false );
  } else
  {
    // If it's not a leaf, go to that child.
    result = yabtl_insert_recursive( tree, ( yabtl_node **)&( *node )->child[i], to_be_inserted, left_child, right_child, key, data );
    if ( *to_be_inserted == NULL )
    {
      return result;
    }

    // Need to add the child's overflowed item here.
    found = false;
    index = yabtl_binary_search( tree, *node, key );
    if ( index < 0 )
    {
      // Should always be less than 0, otherwise we actually found this item?
      index = index * -1 - 1;
    }
    result = *to_be_inserted;
    return yabtl_insert_item_in_node( tree, node, to_be_inserted, left_child, right_child, result, index, true );
  }
}

// Wrapper for recursive insert function.
yabtl_item *yabtl_insert
(
  yabtl *tree,
  void *key,
  void *data
)
{
  yabtl_node *left_child, *right_child;
  yabtl_item *result;
  yabtl_item *to_be_inserted;
  to_be_inserted = NULL;

  result = yabtl_insert_recursive( tree, &tree->root, &to_be_inserted, &left_child, &right_child, key, data );
  if ( to_be_inserted != NULL )
  {
    // Need to create a new root since this is full and insert the item.
    tree->root = malloc( sizeof( yabtl_node ) );
    yabtl_init_node( tree, &tree->root );
    yabtl_insert_item( tree, &tree->root, 0, to_be_inserted );

    // Set the child pointers.
    tree->root->child[0] = left_child;
    tree->root->child[1] = right_child;

    // Update the leaf flag.
    tree->root->leaf = false;
  }

  int i;
  printf( "Top: " );
  for ( i = 0; i < tree->root->count; i++ )
  {
    printf( "%d => ", *( int * )tree->root->item[i]->key );
  }
  printf( "\n" );
  int j;
  yabtl_node *node;
  for ( i = 0; i < tree->root->count + 1; i++ )
  {
    if ( tree->root->child[i] == NULL )
      break;
    printf( "Child %d: ", i );
    node = tree->root->child[i];
    for ( j = 0; j < node->count; j++ )
      printf( "%d => ", *( int * ) node->item[j]->key );
    printf( "\n" );
  }
printf( "\n" );
  return result;
}
