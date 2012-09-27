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
    default:
      break;
  }

  YABTL_STRING,
  YABTL_INT,
  YABTL_UINT32_T,
  YABTL_UINT8_t,
  YABTL_CHAR,
  YABTL_UNSIGNED_CHAR,
  YABTL_LONG,
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
  node->child = calloc( tree->items_per_node + 1, sizeof( yabtl_node * ) );
  node->item = calloc( tree->items_per_node, sizeof( yabtl_item * ) );

  // Initialize our child and item pointers.
  for ( i = 0; i < tree->items_per_node; i++ )
  {
    node->child[i] = NULL;
    node->item[i].data = NULL;
    node->item[i].key = NULL;
  }
  node->child[i] = NULL;

  // Return it.
  return node;
}

// Initialize a b-tree with <count> items per node.
void yabtl_init( yabtl *tree, uint32_t count )
{
  yabtl_node *node;

  // Set our items per node.
  tree->items_per_node = count;

  // Create our root node.
  node = yabtl_allocate_node( tree );

  // Point the tree's root to this new node.
  tree->root = node;
}

// Split a child node.
bool yabtl_split_child( yabtl *tree, yabtl_node *node, uint32_t index )
{
  uint32_t i;

  yabtl_node *new_node;
  yabtl_node *child;

  new_node = yabtl_allocate_node( tree );
  child = node->child[index];

  new_node->leaf = child->leaf;
  new_node->count = tree->items_per_node - 1;

  for ( i = 0; i < tree->items_per_node - 1; i++ )
  {
    new_node->item[i].key = ( ( yabtl_node * )child )->item[i + 1].key;
    new_node->item[i].data = ( ( yabtl_node * )child )->item[i + 1].data;
  }

  if ( child->leaf == false )
  {
    for ( i = 0; i < tree->items_per_node; i++ )
    {
      new_node->child[i] = child->child[i + 1];
    }
  }

  child->count = tree->items_per_node - 1;

  for ( i = node->count; i > index; i-- )
  {
    node->child[i + 1] = node->child[i];
  }

  node->child[index] = new_node;

  for ( i = node->count; i > index; i-- )
  {
    node->item[i + 1].key = node->item[i].key;
  }
  node->item[i + 1].key = child->item[tree->items_per_node - 1].key;
  node->count = node->count++;
}

// Insert an item into a node.
yabtl_item *yabtl_insert_partial_full( yabtl *tree, yabtl_node *node, void *key, void *data )
{
  uint32_t i;
  yabtl_node *child;

  i = node->count - 1;
  if ( node->leaf == true )
  {
    while ( i >= 0 && yabtl_compare( key, node->item[i].key ) == LESS_THAN )
    {
      node->item[i + 1].key = node->item[i].key;
      node->item[i + 1].data = node->item[i].data;
      i--;
    }
    yabtl_copy_key( tree, &node->item[i + 1], key );
    node->item[i + 1].data = data;
    node->count++;
    return &node->item[i + 1];
  } else
  {
    while ( i >= 0 && yabtl_compare( key, node->item[i].key ) == LESS_THAN )
    {
      i--;
    }
    i++;
    child = ( yabtl_node * )node->child[i];
    if ( child->count == tree->items_per_node )
    {
      yabtl_split_child( tree, child, i );
      if ( yabtl_compare( key, node->item[i].key ) == GREATER_THAN )
      {
        i++;
      }
    }
    return yabtl_insert_partial_full( tree, node->child[i], key, data );
  }
}

// Insert a node into the tree.
yabtl_node *yabtl_insert( yabtl *tree, void *key, void *data )
{
  yabtl_node *node;
  yabtl_node *new_node;

  node = tree->root;
  if ( node->count == tree->items_per_node )
  {
    new_node = yabtl_allocate_node( tree );
    tree->root = new_node;
    new_node->leaf = false;
    new_node->count = 0;
    new_node->child[0] = node;
    yabtl_split_child( tree, new_node, 0 );
    yabtl_insert_partial_full( tree, new_node, key, data );
  } else
  {
    yabtl_insert_partial_full( tree, node, key, data );
  }
}
