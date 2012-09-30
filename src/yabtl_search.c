#include <yabtl.h>

// yabtl_compare is our wrapper comparison function, it must be pointed to a function by the client.
yabtl_cmp ( *yabtl_compare )( void *, void * );

yabtl_item *yabtl_search( yabtl_node *node, void *key )
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
  while ( i < node->count && ( ( result = yabtl_compare( node->item[i]->key, key ) ) == LESS_THAN ) )
  {
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
    return yabtl_search( ( yabtl_node * )node->child[i], key );
  }
}
