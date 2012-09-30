#include <yabtl.h>

int main( int argc, char *argv[] )
{
  yabtl tree;
  int key = 27;
  int data = 14;
  int i, j;
  yabtl_item *item;

  yabtl_init( &tree, 3, YABTL_INT );
  for ( i = 0; i < 4; i++ )
  {
    key = 4 - i;
    data = i;
    item = yabtl_insert( &tree, &tree.root, ( void * )&key, ( void * )&data );
    printf( "Inserted: %d\n", key );
  }

printf( "Root count: %d\n", tree.root->count );
for ( i = 0; i < tree.root->count; i++ )
  printf( "%d -> ", *( int * )tree.root->item[i]->key );
printf( "\n" );

  return 0;
}
