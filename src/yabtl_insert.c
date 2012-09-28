#include <yabtl.h>

void yabtl_copy_key( yabtl *tree, yabtl_item *item, void *key )
{
  char *temp_key;

  switch ( tree->key_type )
  {
    case YABTL_STRING:
      temp_key = ( void * )( strdup( ( char * )key ) );
      break;
    case YABTL_INT:
      temp_key = malloc( sizeof( int ) );
      *temp_key = *( int * )key;
      break;
    case YABTL_UINT32_T:
      temp_key = malloc( sizeof( uint32_t ) );
      *temp_key = *( uint32_t * )key;
      break;
    case YABTL_UINT8_T:
      temp_key = malloc( sizeof( uint8_t ) );
      *temp_key = *( uint8_t * )key;
      break;
    case YABTL_CHAR:
      temp_key = malloc( sizeof( char ) );
      *temp_key = *( char * )key;
      break;
    case YABTL_UNSIGNED_CHAR:
      temp_key = malloc( sizeof( unsigned char ) );
      *temp_key = *( unsigned char * )key;
      break;
    case YABTL_LONG:
      temp_key = malloc( sizeof( long ) );
      *temp_key = *( long * )key;
      break;
    default:
      break;
  }
  item->key = temp_key;
}

// Allocate a node.
yabtl_node *yabtl_allocate_node( yabtl *tree )
{
  uint32_t i;
  yabtl_node *node;

  // Allocate memory.
  node = malloc( sizeof( yabtl_node ) );
  if ( node == NULL )
  {
    // Error allocating memory.
    return NULL;
  }

  // Initialize the node, allocate memory for child and item pointers.
  node->leaf = true;
  node->count = 0;
  node->child = calloc( tree->order, sizeof( yabtl_node * ) );
  node->item = calloc( tree->order - 1, sizeof( yabtl_item * ) );

  // Initialize our child and item pointers.
  for ( i = 0; i < tree->order - 1; i++ )
  {
    node->child[i] = NULL;
    node->item[i].data = NULL;
    node->item[i].key = NULL;
  }
  node->child[i] = NULL;

  // Return it.
  return node;
}

// Set the comparison function pointer.
void yabtl_set_compare_pointer( yabtl *tree )
{
  switch ( tree->key_type )
  {
    case YABTL_STRING:
      tree->compare = &yabtl_compare_string;
      break;
    case YABTL_INT:
      tree->compare = &yabtl_compare_int;
      break;
    case YABTL_UINT32_T:
      tree->compare = &yabtl_compare_uint32_t;
      break;
    case YABTL_UINT8_T:
      tree->compare = &yabtl_compare_uint8_t;
      break;
    case YABTL_CHAR:
      tree->compare = &yabtl_compare_char;
      break;
    case YABTL_UNSIGNED_CHAR:
      tree->compare = &yabtl_compare_unsigned_char;
      break;
    case YABTL_LONG:
      tree->compare = &yabtl_compare_unsigned_char;
      break;
    default:
      tree->compare = &yabtl_compare_int;
      break;
  }
}

// Initialize a b-tree.
void yabtl_init( yabtl *tree, uint32_t order, yabtl_key_type key_type )
{
  yabtl_node *node;

  // Set our items per node.
  tree->order = order;
  tree->key_type = key_type;
  yabtl_set_compare_pointer( tree );

  // Create our root node.
  node = yabtl_allocate_node( tree );

  // Point the tree's root to this new node.
  tree->root = node;
}

// Binary search to find location of node.
int yabtl_binary_search( yabtl *tree, yabtl_node *node, void *key )
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

printf( "Count: %d, min: %d, max: %d, mid: %d\n", node->count, min, max, mid );
    // Get the corresponding key.
    mid_key = node->item[mid].key;

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
void yabtl_insert_item( yabtl *tree, yabtl_node **node, int index, void *key, void *data )
{
  int j;

  // Insert directly, no need to split.
  for ( j = ( *node )->count; j > index; j-- )
  {
    ( *node )->item[j + 1] = ( *node )->item[j];
  }
  ( *node )->count++;

  // Copy the key and set data.
  yabtl_copy_key( tree, &( *node )->item[index], key );
  ( *node )->item[index].data = data;
}

// Insert an item into the tree.
yabtl_item *yabtl_insert_recursive( yabtl *tree, yabtl_node **node, yabtl_item **to_be_inserted, void *key, void *data )
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
    return &( *node )->item[index];
  }

  if ( ( *node )->leaf == true )
  {
    // If the node is a leaf, try to insert it.
    if ( ( *node )->count < tree->order )
    {
      // Insert directly, no need to split.
      yabtl_insert_item( tree, node, index, key, data );

      // We don't have a node to insert into the parent node.
      *to_be_inserted = NULL;

      // Return this item.
      return &( *node )->item[index];
    }

    // If we got here, we need to split this node.
    new_node = yabtl_allocate_node( tree );
    mid = ( ( *node )->count >> 1 );

    // Move half of the nodes to the new node.
    for ( i = mid; i < ( *node )->count; i++ )
    {
      // Move the key.
      temp = ( *node )->item[i].key;
      new_node->item[i - mid].key = temp;
      ( *node )->item[i].key = NULL;

      // Move the data.
      temp = ( *node )->item[i].data;
      new_node->item[i - mid].data = temp;
      ( *node )->item[i].data = NULL;
    }

    // Insert the new item into the left or right node.
    if ( index < mid )
    {
      yabtl_insert_item( tree, node, index, key, data );
      *to_be_inserted = &( *node )->item[index];
    } else if ( index > mid )
    {
      i = yabtl_binary_search( tree, new_node, key );
      yabtl_insert_item( tree, &new_node, i, key, data );
      *to_be_inserted = &new_node->item[i];
    } else
    {
      yabtl_copy_key( tree, *to_be_inserted, key );
      ( *to_be_inserted )->data = data;
    }
printf( "To be inserted: %d\n", *( int * )( ( *to_be_inserted )->key ) );
    // Set the to_be_inserted node to our mid.

  } else
  {
    // If it's not a leaf, go to that child.
    result = yabtl_insert_recursive( tree, ( yabtl_node **)&( *node )->child[i], to_be_inserted, key, data );
    if ( *to_be_inserted == NULL )
    {
      return result;
    }

    // If to_be_inserted was set, insert that node into this one.
    printf( "Need to add: %d\n", *( int * )( *to_be_inserted )->key );
  }
}

// Wrapper for recursive insert function.
yabtl_item *yabtl_insert( yabtl *tree, yabtl_node **node, void *key, void *data )
{
  yabtl_item *result;
  yabtl_item to_be_inserted[1];

  result = yabtl_insert_recursive( tree, node, ( yabtl_item ** )&to_be_inserted, key, data );

  if ( to_be_inserted != NULL )
  {
    
  }
  return result;
}
