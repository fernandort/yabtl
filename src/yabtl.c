#include <yabtl.h>
#include <sys/time.h>

int main( int argc, char *argv[] )
{
  yabtl tree;
  int i;
  uint32_t v, l;
  void *data;
  struct timeval tv1, tv2;
  yabtl_item *item;
  yabtl_node *iter;
  uint32_t TOTAL = 10000000;
  uint32_t ORDER = 3;

  yabtl_init( &tree, ORDER, YABTL_UINT32_T );
srand( time( NULL ) );
  gettimeofday(&tv1, NULL);
  for ( i = 0; i < TOTAL; i++ )
  {
    v = rand() % TOTAL;
    l = v;
    data = malloc( sizeof( uint32_t ) );
    *( uint32_t * )data = v;
    item = yabtl_insert( &tree, ( void * )&v, data );
    continue;
    if ( data != item->data )
      printf( "Already existed: %d\n", *( uint32_t * )data );
    else
      printf( "Created: %d\n", *( uint32_t * )data );
  }
  gettimeofday(&tv2, NULL);
  printf ("Total time to insert %d items: %f seconds\n", TOTAL, (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +  (double) (tv2.tv_sec - tv1.tv_sec));

  gettimeofday(&tv1, NULL);
  item = yabtl_search( &tree, ( void * )&l );
  gettimeofday(&tv2, NULL);
  if ( item == NULL )
    printf( "Failed to find %d\n", l );
  else
    printf( "Found: %d\n", *( int * )item->key );
  printf ("Total time to query: %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +  (double) (tv2.tv_sec - tv1.tv_sec));
  return 0;
}
