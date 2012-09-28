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
  uint32_t max;
  uint32_t min;
  uint32_t mid;
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
    mid = ( ( min + max ) >> 1 );

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
  return -min;
}

// Insert a node into the tree.
yabtl_item *yabtl_insert( yabtl *tree, yabtl_node **node, void *key, void *data )
{
  int i, j;
  yabtl_cmp cmp;

  // Make sure we weren't passed a null node.
  if ( node == NULL || *node == NULL )
  {
    return NULL;
  }

  // Try to find our insert position.
  i = 0;
  if ( ( *node )->count == 0 )
  {
    i = 0;
    cmp = LESS_THAN;
  } else
  {
    i = yabtl_binary_search( tree, *node, key );
    if ( i >= 0 )
    {
      cmp = EQUAL_TO;
    } else
    {
      cmp = LESS_THAN;
      i = i * -1;
    }
  }

  if ( cmp == EQUAL_TO )
  {
    // If we found the key in this node, return it without updating the data.
    return &( *node )->item[i];
  }

  if ( ( *node )->leaf == true )
  {
   // If the node is a leaf, try to insert it.
    if ( ( *node )->count < tree->order - 1 )
    {
      // Insert directly, no need to split.
      for ( j = ( *node )->count; j > i; j-- )
      {
        ( *node )->item[j + 1] = ( *node )->item[j];
      }
      ( *node )->count++;
      yabtl_copy_key( tree, &( *node )->item[i], key );
      ( *node )->item[i].data = data;
      return &( *node )->item[i];
    }

    // If we got here, we need to split this node.
    
    printf( "O noes!  I don't want to split a node!\n" );
  } else
  {
    // If it's not a leaf, go to that child.
    return yabtl_insert( tree, ( yabtl_node **)&( *node )->child[i], key, data );
  }
}
