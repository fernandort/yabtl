#include <yabtl.h>

// Find an item within our b-tree.
yabtl_item *yabtl_search_recursive
(
  yabtl *tree,
  yabtl_node *node,
  void *key
)
{
  uint32_t i;
  yabtl_cmp result;

  // Some basic checks before continuing.
  if ( node == NULL || key == NULL || node->count == 0 )
  {
    return NULL;
  }

  // Find the position in this node (or where to go next).
  i = 0;
  while ( i < node->count )
  {
     if ( ( result = tree->compare( node->item[i]->key, key ) ) != LESS_THAN )
     {
       break;
     }
     i++;
  }

  if ( i < node->count && result == EQUAL_TO )
  {
    // We found the item, return it.
    return node->item[i];
  } else if ( node->leaf == true )
  {
    // We can't check any further down the tree, item doesn't exist.
    return NULL;
  } else
  {
    // Need to check the child of this node.
    return yabtl_search_recursive( tree, ( yabtl_node * )node->child[i], key );
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
