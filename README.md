#yabtl - A fast, versatile memory-only b-tree library written in c.

## Initializing a b-tree.
```c
  #include <yabtl.h>
  ...
  yabtl tree;
  yabtl_init( &tree, 3, YABTL_STRING );

  /*********************************************************
    yabtl_init second argument is children per node:
      Determines how many items are in each node as well
      as the number of comparisons per node required.

    yabtl_init third argument is the data type of the key.
      The data type of the key can be one of the following:
      YABTL_STRING - char *
      YABTL_INT - int
      YABTL_UINT32_T - uint32_t
      YABTL_UINT8_T - uint8_t
      YABTL_CHAR - char
      YABTL_UNSIGNED_CHAR - unsigned char
      YABTL_LONG - long
  *********************************************************/
```

## Searching for an item.
```c
  ...
  yabtl_item *item;
  char *key = "test_key";

  if ( ( item = yabtl_search( &tree, ( void * )key ) ) != NULL )
  {
    printf( "Found %s\n", key );
  } else
  {
    printf( "Could not find %s\n", key );
  }
```

## Inserting an item.
```c
  ...
  yabtl_item *item;
  char *key = "test_key";
  item = yabtl_insert( &tree, ( void * )key, NULL );

  /*********************************************************
    yabtl_item *yabtl_insert( yabtl *tree, void *key, void *data );
    Note that if you pass a non-NULL data pointer, it will be freed
      upon destroying the item or the tree.
  *********************************************************/

  ...
  int *data;
  data = malloc( sizeof( int ) );
  *data = 10;

  char *key = "test_key_2";
  item = yabtl_insert( &tree, ( void * )key, ( void * )data );

  /*********************************************************
    Insertions do not overwrite data, instead if the item
     already exists, it returns the existing item.

    You can check if the item already exists with:
  *********************************************************/

  ...
  char *key = "test_key_3";
  void *data;
  data = malloc( sizeof( int ) );
  *( int * )data = 5;

  item = yabtl_insert( &tree, ( void * )key, data );
  if ( item->data != data )
  {
    printf( "%s already existed!\n", key );
  }
```

## Deleting an item.
```c
  ...
  yabtl_delete( &tree, ( void * )key );
```

## Destroying a tree.
```c
  yabtl_destroy( &tree );
```
