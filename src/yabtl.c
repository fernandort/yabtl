#include <yabtl.h>
#include <sys/time.h>

#define COUNT 3
#define TOTAL 10 //10000

int main( int argc, char *argv[] )
{
  yabtl tree;
  int i, v;
  struct timeval tv1, tv2;
  yabtl_item *item;
  yabtl_node *iter;

  yabtl_init( &tree, COUNT, YABTL_INT );

  gettimeofday(&tv1, NULL);
  for ( i = 0; i < TOTAL + 1; i++ )
  {
    v = i; //TOTAL - i; //i; //10 - i;
    item = yabtl_insert( &tree, ( void * )&v, ( void * )&i );
  }
  gettimeofday(&tv2, NULL);
  printf ("Total time = %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +  (double) (tv2.tv_sec - tv1.tv_sec));

  i = 4;
  gettimeofday(&tv1, NULL);
  item = yabtl_search( &tree, ( void * )&i );
  gettimeofday(&tv2, NULL);
  if ( item == NULL )
    printf( "Failed to find %d\n", i );
  else
    printf( "Found: %d\n", *( int * )item->key );
  printf ("Total time = %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +  (double) (tv2.tv_sec - tv1.tv_sec));

  return 0;
}
