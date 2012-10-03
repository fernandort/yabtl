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

  // Free the item's key.
  if ( ( *item )->key != NULL )
  {
    free( ( *item )->key );
    ( *item )->key = NULL;
  }

  // Free the data if it's not NULL.
  if ( ( *item )->data != NULL )
  {
    free( ( *item )->data );
    ( *item )->data = NULL;
  }

  // Free the item.
  free( *item );
  *item = NULL;
}

// Take an item from a sibling.
void yabtl_shift
(
  yabtl *tree,
  yabtl_node **parent,
  yabtl_node **left,
  yabtl_node **right,
  int index,
  bool shift_left
)
{
  int i;

  if ( shift_left == true )
  {
    // Move the parent item to the left node.
    ( *left )->item[( *left )->count++] = ( *parent )->item[index];

    // Move the first item in the right node to parent.
    ( *parent )->item[index] = ( *right )->item[0];

    // Move the first child in right node to left node.
    ( *left )->child[( *left )->count] = ( *right )->child[0];

    // Shift over all the items and children in right node.
    for ( i = 0; i < ( *right )->count; i++ )
    {
      ( *right )->item[i] = ( *right )->item[i + 1];
      ( *right )->child[i] = ( *right )->child[i + 1];
    }
    ( *right )->child[i] = ( *right )->child[i + 1];
  } else
  {
    // Shift all the items in the right node over.
    for ( i = ( *right )->count; i > 0; i++ )
    {
      ( *right )->item[i] = ( *right )->item[i - 1];
      ( *right )->child[i] = ( *right )->child[i - 1];
    }
    ( *right )->child[i] = ( *right )->child[i - 1];

    // Move the parent item to right.
    ( *right )->item[0] = ( *parent )->item[index];
    ( *parent )->item[index] = ( *left )->item[( *left )->count - 1];
    ( *right )->child[0] = ( *left )->child[( *left )->count];

    // Nullify the items that were moved from left.
    ( *left )->child[( *left )->count] = NULL;
    ( *left )->item[( *left )->count - 1] = NULL;

    // Decrement count.
    ( *left )->count--;
  }
}

// Merge two nodes into one.
void yabtl_merge_siblings
(
  yabtl *tree,
  yabtl_node **destination,
  yabtl_node **left,
  yabtl_node **right,
  yabtl_item *parent_item
)
{
  int i;
  int index;

  // Move all of the items from the left sibling into the new node.
  for ( i = 0; i < ( *left )->count; i++ )
  {
    ( *destination )->item[i] = ( *left )->item[i];
    ( *destination )->child[i] = ( *left )->child[i];
    ( *left )->item[i] = NULL;
    ( *left )->child[i] = NULL;
  }
  ( *destination )->child[i] = ( *left )->child[i];
  ( *left )->child[i] = NULL;
  ( *destination )->count = ( *left )->count;

  // Move all of the items from the right sibling into the new node.
  for ( i = 0; i < ( *right )->count; i++ )
  {
    ( *destination )->item[( *destination )->count + i] = ( *right )->item[i];
    ( *destination )->child[( *destination )->count + i + 1] = ( *right )->child[i];
    ( *right )->item[i] = NULL;
    ( *right )->child[i] = NULL;
  }
  ( *destination )->child[( *destination )->count + i + 1] = ( *right )->child[i];
  ( *right )->child[i] = NULL;
  ( *destination )->count += ( *right )->count;

  // Insert the parent item.
  index = yabtl_binary_search( tree, *destination, parent_item->key ) * -1 - 1;
  yabtl_insert_item( tree, destination, index, parent_item );

  // Destroy the two original nodes.
  yabtl_destroy_node( left );
  yabtl_destroy_node( right );
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
  int left_index, right_index;
  yabtl_node *left, *right;
  yabtl_node *new_node;
  yabtl_item *parent_item;

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
      ( *node )->item[i] = NULL;

      // Decrement count.
      ( *node )->count--;

      // Return true, indicating we successfully removed an item.
      return true;
    } else
    {
      // This is an internal node, need to do more stuff...
      printf( "O noes, I'm in an internal node!  What a PITA\n" );
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
      if ( ( ( yabtl_node * )( *node )->child[index] )->count >= ( tree->order >> 1 ) )
      {
        printf( "All done, child still has %d items.\n", ( ( yabtl_node * )( *node )->child[index] )->count );
        return result;
      }

      // Set pointers to left and right siblings.
      if ( index - 1 >= 0 )
      {
        left = ( yabtl_node * )( *node )->child[index - 1];
      } else
      {
        left = NULL;
      }
      right = ( yabtl_node * )( *node )->child[index + 1];

      if ( left != NULL && left->count > ( tree->order >> 1 ) )
      {
        // Shift an item from the left sibling into the current node.
        yabtl_shift( tree, node, ( yabtl_node ** )&( *node )->child[index - 1], ( yabtl_node ** )&( *node )->child[index], index == ( *node )->count ? index - 1 : index, false );
      } else if ( right != NULL && right->count > ( tree->order >> 1 ) )
      {
        // Shift an item from the right sibling into the current node.
        yabtl_shift( tree, node, ( yabtl_node ** )&( *node )->child[index], ( yabtl_node ** )&( *node )->child[index + 1], index == ( *node )->count ? index - 1 : index, true );
      } else
      {
        // If we can't borrow from a sibling, we need to merge two nodes, set the index of left and right siblings.
        if ( left != NULL )
        {
          left_index = index - 1;
          right_index = index;
        } else
        {
          left_index = index;
          right_index = index + 1;
        }

        // Create a new node.
        new_node = malloc( sizeof( yabtl_node ) );
        yabtl_init_node( tree, &new_node );

        // Fix index of parent item to be moved.
        if ( index > 0 )
        {
          index--;
        }
        // Set the pointer to the item that will be moved.
        parent_item = ( *node )->item[index];

        // Merge the siblings.
        yabtl_merge_siblings( tree, &new_node, ( yabtl_node ** )&( *node )->child[left_index], ( yabtl_node ** )&( *node )->child[right_index], parent_item );

        // Remove the parent item.
        for ( i = index; i < ( *node )->count; i++ )
        {
          ( *node )->item[i] = ( *node )->item[i + 1];
        }

        // Move children over and insert new one.
        ( *node )->child[index] = new_node;
        for ( i = index + 1; i < ( *node )->count + 1; i++ )
        {
          ( *node )->child[i] = ( *node )->child[i + 1];
        }
      }
    }
    return result;
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
