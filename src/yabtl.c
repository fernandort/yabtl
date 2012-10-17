#include <yabtl.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <signal.h>

#define TOTAL 1000000
#define ORDER 1000

// Semaphore to start timing.
sem_t timer_lock;

// B-tree for timed insert.
yabtl timed_tree;

// Cache the interrupt signal for the timed insert.
static void catch_signal( int signal )
{
  // No-op.
}

// Run inserts for 1 second to see how many we can get.
void *timed_inserts( void *args )
{
  uint32_t i;

  // Start inserting.
  i = 0;
  sem_wait( &timer_lock );
  while ( true )
  {
    ( void )yabtl_insert( &timed_tree, ( void * )&i, NULL );
    i++;
  }
}

// YABTL example.
int main( int argc, char *argv[] )
{
  pthread_t thread;
  yabtl tree, timed;
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
    yabtl_insert( &tree, ( void * )&i, NULL );

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

  yabtl_iterator iter;
  yabtl_init_iterator( &tree, &iter );
  gettimeofday( &tv1, NULL );
  while ( ( item = yabtl_iterate( &iter ) ) != NULL )
  {
    // Do something with item here.
  }
  gettimeofday( &tv2, NULL );
  printf ("Total time to iterate all items: %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec)/1000000 +  (double) (tv2.tv_sec - tv1.tv_sec));

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

  // Real stress-test - Insert as many items as possible in 1 second.
  sem_init( &timer_lock, 0, 1 );
  sem_wait( &timer_lock );
  yabtl_init( &timed_tree, ORDER, YABTL_UINT32_T );
  printf( "Performing timed insert.\n" );
  pthread_create( &thread, NULL, timed_inserts, NULL );
  signal( SIGINT, catch_signal );
  sem_post( &timer_lock );
  sleep( 1 );
  pthread_kill( thread, SIGINT );
  i = 0;
  yabtl_init_iterator( &timed_tree, &iter );
  while ( ( item = yabtl_iterate( &iter ) ) != NULL )
    i++;
  printf( "Inserted %d items in 1 second\n", i );
  return 0;
}
