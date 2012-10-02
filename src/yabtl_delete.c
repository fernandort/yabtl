#include <yabtl.h>

// Delete a single item from a node.
void yabtl_destroy_item
(
  yabtl_item **item
)
{
  // Make sure the item isn't null already.
  if ( *item == NULL )
  {
    return;
  }

  // Free the item's parts.
  free( ( *item )->key );
  free( ( *item )->data );

  // Make sure we don't have hanging pointers.
  ( *item )->key = NULL;
  ( *item )->data = NULL;

  // Free the item.
  free( *item );
  *item = NULL;
}

bool yabtl_delete_recursive
(
  yabtl *tree,
  yabtl_node **node,
  void *key
)
{
  int i;
  int index;
  bool result;

  index = yabtl_binary_search( tree, *node, key );

  if ( index >= 0 )
  {
    // Found the item.
    if ( ( *node )->leaf == true )
    {
      // We're in a leaf node, so just remove it.
      yabtl_destroy_item( &( *node )->item[index] );
      for ( i = index; i < ( *node )->count - 1; i++ )
      {
        ( *node )->item[i] = ( *node )->item[i + 1];
      }

      // Decrement count.
      ( *node )->count--;
      return true;
    } else
    {
      // This is an internal node, need to do more stuff...
      printf( "O noes, I'm in an internal node!\n" );
    }
  } else
  {
    if ( ( *node )->leaf == true )
    {
      // We are in a leaf node, couldn't find it, return false.
      return false;
    }

    // We didn't find the item, check child at the closest index.
    index = index * -1 - 1;
    if ( ( result = yabtl_delete_recursive( tree, ( yabtl_node ** )&( *node )->child[index], key ) ) == true )
    {
      printf( "Child count: %d\n", ( ( yabtl_node * )( *node )->child[index] )->count );
      printf( "Need to have at least %d\n", tree->order - 1 );
      // We found the item and deleted it.
      printf( "Found and removed, weeee!\n" );
    }
  }
}

// Delete recursive wrapper.
bool yabtl_delete
(
  yabtl *tree,
  void *key
)
{
  return yabtl_delete_recursive( tree, &tree->root, key );
}

// Delete an entire node from the tree.
void yabtl_destroy_node
(
  yabtl_node **node
)
{
  int i;

  // Make sure the node isn't already null.
  if ( *node == NULL )
  {
    return;
  }

  // Delete all of the child sub-trees.
  for ( i = 0; i < ( *node )->count + 1; i++ )
  {
    if ( ( *node )->child[i] != NULL )
    {
      yabtl_destroy_node( ( yabtl_node ** )&( *node )->child[i] );
    }
  }
  free( ( *node )->child );
  ( *node )->child = NULL;

  // Delete all of the items.
  for ( i = 0; i < ( *node )->count; i++ )
  {
    if ( ( *node )->item[i] != NULL )
    {
      yabtl_destroy_item( ( yabtl_item ** )&( *node )->item[i] );
    }
  }
  free( ( *node )->item );
  ( *node )->item = NULL;

  // Free the memory used by the node.
  free( *node );
  *node = NULL;
}

// Recursively destroy an entire b-tree.
void yabtl_destroy
(
  yabtl *tree
)
{
  // Make sure we weren't passed a null tree.
  if ( tree == NULL )
  {
    return;
  }

  // Start at the root node and work our way down.
  yabtl_destroy_node( &tree->root );
}
