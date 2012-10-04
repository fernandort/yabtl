#include <yabtl.h>

void yabtl_init_iterator
(
  yabtl *tree,
  yabtl_iterator *iterator
)
{
  if ( tree->root == NULL || tree->root->count == 0 )
  {
    iterator->stack = NULL;
    return;
  }

  // Create the stack and insert the root node.
  iterator->stack = malloc( sizeof( struct yabtl_iterator_stack ) );
  iterator->stack->next = NULL;
  iterator->stack->index = 0;
  iterator->stack->child_next = true;
  iterator->stack->node = tree->root;
}

// Pop an item off the iterator stack.
void yabtl_iterator_pop
(
  yabtl_iterator *iterator
)
{
  struct yabtl_iterator_stack *next;
  next = iterator->stack->next;
  free( iterator->stack );
  iterator->stack = next;
}

// Iterate a tree.
yabtl_item *yabtl_iterate
(
  yabtl_iterator *iterator
)
{
  struct yabtl_iterator_stack *iter;
  struct yabtl_iterator_stack *next;

  // If stack is null, no more items, done iterating.
  if ( iterator->stack == NULL )
  {
    return NULL;
  }

  iter = iterator->stack;
  if ( iter->node->leaf == true )
  {
    iter->index++;
    if ( iter->index > iter->node->count )
    {
      // No more items to visit, pop this node off the stack and re-iterate.
      yabtl_iterator_pop( iterator );
      return yabtl_iterate( iterator );
    }

    // We have a non-null item, return it.
    return iter->node->item[iter->index - 1];
  } else
  {
    if ( iter->child_next == true )
    {
      // Set this flag so we visit the item next.
      iter->child_next = false;

      if ( iter->node->child[iter->index] != NULL )
      {
        // Push the child onto the stack and re-iterate.
        iter->child_next = false;
        next = malloc( sizeof( struct yabtl_iterator_stack ) );
        next->index = 0;
        next->child_next = true;
        next->node = iter->node->child[iter->index];
        next->next = iter;
        iterator->stack = next;
        return yabtl_iterate( iterator );
      }

      // No more children to visit, pop this node off the stack and iterate.
      yabtl_iterator_pop( iterator );
      return yabtl_iterate( iterator );
    } else
    {
      iter->index++;
      iter->child_next = true;
      if ( iter->index > iter->node->count )
      {
        // No more items left.
        yabtl_iterator_pop( iterator );
        return yabtl_iterate( iterator );
      }
      return iter->node->item[iter->index - 1];
    }
  }
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
