#include <yabtl.h>
#include <sys/time.h>

int main( int argc, char *argv[] )
{
  yabtl tree;
  int i;
  uint32_t v, l;
  struct timeval tv1, tv2;
  yabtl_item *item;
  yabtl_node *iter;
  uint32_t TOTAL = 1000; //4194304;
  uint32_t ORDER = 17;

  yabtl_init( &tree, ORDER, YABTL_UINT32_T );
  srand( time( NULL ) );
  gettimeofday(&tv1, NULL);
  for ( i = 0; i < TOTAL; i++ )
  {
    v = i; //rand() % TOTAL;
    l = v;
    void *data = malloc( sizeof( uint32_t ) );
    *( uint32_t * )data = v;
    item = yabtl_insert( &tree, ( void * )&v, data );
    if ( item->data != data )
    {
      free( data );
      data = NULL;
    }
  }
  gettimeofday(&tv2, NULL);
  printf ("Total time to insert %d items: %f seconds\n", TOTAL, (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +  (double) (tv2.tv_sec - tv1.tv_sec));

  gettimeofday(&tv1, NULL);
  item = yabtl_search( &tree, ( void * )&l );
  gettimeofday(&tv2, NULL);
  if ( item == NULL )
    printf( "Failed to find %d\n", l );
  else
    printf( "Found key: %d\n", *( int * )item->key );
  printf ("Total time to query: %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +  (double) (tv2.tv_sec - tv1.tv_sec));

yabtl_iterate( &tree, tree.root );
printf( "\n" );

printf( "Before destroying...\n" );
//sleep( 15 );
yabtl_destroy( &tree );
printf( "Done destroying...\n" );
//sleep( 15 );

  return 0;
}
