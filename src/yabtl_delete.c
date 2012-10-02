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

// Merge the node origin into the node destination.
void yabtl_merge_siblings
(
  yabtl_node **parent,
  yabtl_node **destination,
  yabtl_node **origin,
  int index,
  bool merging_left
)
{
  int i;
  yabtl_item *to_move;

  // First, insert the parent item at index into the destination node.
  if ( merging_left == true )
  {
    index--;
    to_move = ( *parent )->item[index];
  } else
  {
    to_move = ( *parent )->item[index];
  }
  ( *destination )->item[( *destination )->count++] = to_move;

printf( "Moving %d from parent.\n", *( int * )to_move->key );
printf( "Shifting elements from %d+ over.\n", index );

  // Remove the item at index from parent.
  for ( i = index; i < ( *parent )->count + 1; i++ )
  {
    ( *parent )->item[i] = ( *parent )->item[i + 1];
    ( *parent )->child[i] = ( *parent )->child[i + 1];
  }
  ( *parent )->child[i] = ( *parent )->child[i + 1];
  ( *parent )->count--;

  // Move all of the children over.
  for ( i = 0; i < ( *origin )->count + 1; i++ )
  {
    ( *destination )->child[( *destination )->count + i] = ( *origin )->child[i];
    ( *origin )->child[i] = NULL;
  }

  // Add all of the items and children from origin to destination.
  for ( i = 0; i < ( *origin )->count; i++ )
  {
    ( *destination )->item[( *destination )->count++] = ( *origin )->item[i];
    ( *origin )->item[i] = NULL;
  }

  // Destroy the origin.
  yabtl_destroy_node( origin );
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
  yabtl_node *left, *right;

  index = yabtl_binary_search( tree, *node, key );

  if ( index >= 0 )
  {
    // Found the item.
    if ( ( *node )->leaf == true )
    {
      // We're in a leaf node, so just remove it.
      yabtl_destroy_item( &( *node )->item[index] );
      for ( i = index; i < ( *node )->count; i++ )
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
      if ( ( ( yabtl_node * )( *node )->child[index] )->count >= tree->order - 1 )
      {
        printf( "All done, child still has %d items.\n", ( ( yabtl_node * )( *node )->child[index] )->count );
        return result;
      }
      if ( index - 1 >= 0 )
      {
        left = ( yabtl_node * )( *node )->child[index - 1];
      } else
      {
        left = NULL;
      }
      right = ( yabtl_node * )( *node )->child[index + 1];
      if ( left != NULL && left->count >= tree->order )
      {
        printf( "About to borrow from left sibling!\n" );
      } else if ( right != NULL && right->count >= tree->order )
      {
        printf( "About to borrow from right sibling!\n" );
      } else if ( left != NULL )
      {
        printf( "About to merge with left sibling!\n" );
        yabtl_merge_siblings( node, ( yabtl_node ** )&( *node )->child[index - 1], ( yabtl_node ** )&( *node )->child[index], index, true );
      } else if ( right != NULL )
      {
printf( "Origin: %d, Destination: %d\n", index - 1, index );
        printf( "About to merge with right sibling!\n" );
        yabtl_merge_siblings( node, ( yabtl_node ** )&( *node )->child[index], ( yabtl_node ** )&( *node )->child[index + 1], index, false );
      }
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
