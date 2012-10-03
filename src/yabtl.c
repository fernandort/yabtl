#include <yabtl.h>
#include <sys/time.h>

#define TOTAL 1000000
#define ORDER 3

// YABTL example.
int main( int argc, char *argv[] )
{
  yabtl tree;
  uint32_t i;
  struct timeval tv1, tv2;
  yabtl_item *item;

  // Display our tree info.
  printf( "Initializing a b-tree with %d items per node and %d children per node...\n", ORDER - 1, ORDER );
  printf( "Inserting %d items...\n", TOTAL );

  // Initialize the b-tree.
  yabtl_init( &tree, ORDER, YABTL_UINT32_T );

  // Seed the rand() function with current time.
  srand( time( NULL ) );

  // Start the clock so we can see how long insertion takes.
  gettimeofday(&tv1, NULL);

  // Do TOTAL insertions.
  for ( i = 0; i < TOTAL; i++ )
    ( void * )yabtl_insert( &tree, ( void * )&i, NULL );

  // Stop the clock and display total time.
  gettimeofday(&tv2, NULL);
  printf ("Total time to insert %d items: %f seconds\n", TOTAL, (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +  (double) (tv2.tv_sec - tv1.tv_sec));

  // Check how long a search on the last inserted item takes.
  i = rand() % TOTAL;
  gettimeofday(&tv1, NULL);
  item = yabtl_search( &tree, ( void * )&i );
  gettimeofday(&tv2, NULL);

  // Make sure we found the item.
  if ( item == NULL )
    printf( "Failed to find %d\n", i );
  else
    printf( "Found key: %d\n", *( int * )item->key );

  // Show how long it took.
  printf ("Total time to query: %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +  (double) (tv2.tv_sec - tv1.tv_sec));

  // Delete an item.
  gettimeofday( &tv1, NULL );
  yabtl_delete( &tree, ( void * )&i );
  gettimeofday( &tv2, NULL );
  if ( ( item = yabtl_search( &tree, ( void * )&i ) ) == NULL )
    printf( "Successfully removed %d\n", i );
  else
    printf( "Failed to delete %d!\n", i );
  // Show how long it took.
  printf ("Total time to delete: %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +  (double) (tv2.tv_sec - tv1.tv_sec));

  // Destroy the tree.
  printf( "Destroying b-tree...  " );
  yabtl_destroy( &tree );
  printf( "Done!\n" );

  return 0;
}
