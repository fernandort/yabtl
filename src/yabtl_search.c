#include <yabtl.h>

// Recursively iterate a b-tree.
void yabtl_iterate_recursive
(
  yabtl *tree,
  yabtl_node *node
)
{
  int i;

  if ( node == NULL )
  {
    return;
  }
  for ( i = 0; i < ( tree->order << 1 ); i++ )
  {
    if ( node->child[i] != NULL )
    {
      yabtl_iterate_recursive( tree, ( yabtl_node * )node->child[i] );
    }
    if ( node->item[i] != NULL )
      printf( "%d => ", *( uint32_t * )node->item[i]->key );
  }
}

// Wrapper for iterate_recursive.
void yabtl_iterate
(
  yabtl *tree
)
{
  yabtl_iterate_recursive( tree, tree->root );
}

// Find an item within our b-tree.
yabtl_item *yabtl_search_recursive
(
  yabtl *tree,
  yabtl_node *node,
  void *key
)
{
  int index;

  // Some basic checks before continuing.
  if ( node == NULL || key == NULL || node->count == 0 )
  {
    return NULL;
  }

  index  = yabtl_binary_search( tree, node, key );
  if ( index >= 0 )
  {
    return node->item[index];
  } else
  {
    index = index * -1 - 1;
    return yabtl_search_recursive( tree, ( yabtl_node * )node->child[index], key );
  }
}

// Wrapper to search.
yabtl_item *yabtl_search
(
  yabtl *tree,
  void *key
)
{
  return yabtl_search_recursive( tree, tree->root, key );
}
