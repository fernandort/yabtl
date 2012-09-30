#include <yabtl.h>

// Find an item within our b-tree.
yabtl_item *yabtl_search_recursive
(
  yabtl *tree,
  yabtl_node *node,
  void *key
)
{
  int index;
  yabtl_cmp result;

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
