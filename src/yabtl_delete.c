#include <yabtl.h>

void yabtl_delete_item
(
  yabtl_item **item
)
{
  if ( *item == NULL )
  {
    return;
  }

  free( ( *item )->key );
  free( ( *item )->data );
  ( *item )->key = NULL;
  ( *item )->data = NULL;
  free( *item );
  *item = NULL;
}

void yabtl_delete_node
(
  yabtl_node **node
)
{
  int i;

  if ( *node == NULL )
  {
    return;
  }

  if ( ( *node )->child != NULL )
  {
    for ( i = 0; i < ( *node )->count + 1; i++ )
    {
      if ( ( *node )->child[i] != NULL )
      {
        yabtl_delete_node( ( yabtl_node ** )&( *node )->child[i] );
      }
    }
    free( ( *node )->child );
    ( *node )->child = NULL;
  }

  if ( ( *node )->item != NULL )
  {
    for ( i = 0; i < ( *node )->count; i++ )
    {
      if ( ( *node )->item[i] != NULL )
      {
        yabtl_delete_item( ( yabtl_item ** )&( *node )->item[i] );
      }
    }
    free( ( *node )->item );
    ( *node )->item = NULL;
  }

  free( *node );
  *node = NULL;
}

void yabtl_destroy
(
  yabtl *tree
)
{
  if ( tree == NULL )
  {
    return;
  }
  yabtl_delete_node( &tree->root );
}
