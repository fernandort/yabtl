#include <yabtl.h>

// Compare two strings.
yabtl_cmp yabtl_compare_string
(
  void *key_1,
  void *key_2
)
{
  int result;

  // Use the strcmp function to compare the keys.
  result = strcmp( ( char * )key_1, ( char * )key_2 );
  if ( result == 0 )
  {
    return EQUAL_TO;
  } else if ( result < 0 )
  {
    return LESS_THAN;
  } else
  {
    return GREATER_THAN;
  }
}

// Compare two unsigned strings.
yabtl_cmp yabtl_compare_unsigned_string
(
  void *key_1,
  void *key_2
)
{
  int result;
  int length_1;
  int length_2;
  int cmp_length;
  unsigned char *pch_1;
  unsigned char *pch_2;

  // Calculate the string lengths.
  length_1 = 0;
  pch_1 = key_1;
  while ( *pch_1++ )
  {
    length_1++;
  }
  length_2 = 0;
  pch_2 = key_2;
  while ( *pch_2++ )
  {
    length_2++;
  }

  // Determine how many bytes to compare.
  cmp_length = length_1 >= length_2 ? length_2 : length_1;
  result = memcmp( key_1, key_2, cmp_length + 1 );
  if ( result == 0 )
  {
    return EQUAL_TO;
  } else if ( result < 0 )
  {
    return LESS_THAN;
  } else
  {
    return GREATER_THAN;
  }
}

// Compare the value of two integers.
yabtl_cmp yabtl_compare_int
(
  void *key_1,
  void *key_2
)
{
  if ( *( int * )key_1 == *( int * )key_2 )
  {
    return EQUAL_TO;
  } else if ( *( int * )key_1 < *( int * )key_2 )
  {
    return LESS_THAN;
  } else
  {
    return GREATER_THAN;
  }
}

// Compare the value of two uint32_t ints (unsigned ints in 0-4.3billionish range)
yabtl_cmp yabtl_compare_uint32_t
(
  void *key_1,
  void *key_2
)
{
  if ( *( uint32_t * )key_1 == *( uint32_t * )key_2 )
  {
    return EQUAL_TO;
  } else if ( *( uint32_t * )key_1 < *( uint32_t * )key_2 )
  {
    return LESS_THAN;
  } else
  {
    return GREATER_THAN;
  }
}

// Compare the value of two uint8_t ints (unsigned ints in 0-255 range)
yabtl_cmp yabtl_compare_uint8_t
(
  void *key_1,
  void *key_2
)
{
  if ( *( uint8_t * )key_1 == *( uint8_t * )key_2 )
  {
    return EQUAL_TO;
  } else if ( *( uint8_t * )key_1 < *( uint8_t * )key_2 )
  {
    return LESS_THAN;
  } else
  {
    return GREATER_THAN;
  }
}

// Compare the value of char's
yabtl_cmp yabtl_compare_char
(
  void *key_1,
  void *key_2
)
{
  if ( *( char * )key_1 == *( char * )key_2 )
  {
    return EQUAL_TO;
  } else if ( *( char * )key_1 < *( char * )key_2 )
  {
    return LESS_THAN;
  } else
  {
    return GREATER_THAN;
  }
}

// Compare the value of unsigned char's
yabtl_cmp yabtl_compare_unsigned_char
(
  void *key_1,
  void *key_2
)
{
  if ( *( unsigned char * )key_1 == *( unsigned char * )key_2 )
  {
    return EQUAL_TO;
  } else if ( *( unsigned char * )key_1 < *( unsigned char * )key_2 )
  {
    return LESS_THAN;
  } else
  {
    return GREATER_THAN;
  }
}

// Compare the value of two longs.
yabtl_cmp yabtl_compare_long
(
  void *key_1,
  void *key_2
)
{
  if ( *( long * )key_1 == *( long * )key_2 )
  {
    return EQUAL_TO;
  } else if ( *( long * )key_1 < *( long * )key_2 )
  {
    return LESS_THAN;
  } else
  {
    return GREATER_THAN;
  }
}
