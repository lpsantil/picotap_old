/*
Copyright (c) 2013, Louis P. Santillan <lpsantil@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <assert.h>
#include <gc.h>

struct timeval tap_t0, tap_t1, tap_t;

int timeval_subtract( struct timeval *result,
                      struct timeval *x,
                      struct timeval *y );

int timeval_subtract( struct timeval *result,
                      struct timeval *x,
                      struct timeval *y )
{
   /* Perform the carry for the later subtraction by updating y. */
   if( x->tv_usec < y->tv_usec )
   {
      int nsec = ( y->tv_usec - x->tv_usec ) / 1000000 + 1;
      y->tv_usec -= 1000000 * nsec;
      y->tv_sec += nsec;
   }
   if( x->tv_usec - y->tv_usec > 1000000 )
   {
      int nsec = ( x->tv_usec - y->tv_usec ) / 1000000;
      y->tv_usec += 1000000 * nsec;
      y->tv_sec -= nsec;
   }

   /* Compute the time remaining to wait.
      tv_usec is certainly positive. */
   result->tv_sec = x->tv_sec - y->tv_sec;
   result->tv_usec = x->tv_usec - y->tv_usec;

   /* Return 1 if result is negative. */
   return x->tv_sec < y->tv_sec;
}

int tap_f, tap_length;

void tap_popen( char* f )
{
   tap_f = fileno( popen( f, "r" ) );

   assert( tap_f > 0 );

   tap_length = 0;

   gettimeofday( &tap_t0, NULL );
};

char* tap_buf;
char* tap_buf_arr[ 1024 ];
int tap_buf_length = 0,
    lines = 0;
char* tap_fn;

void tap_pslurp( void )
{
   char* line;

   tap_buf = GC_MALLOC( 64 * 1024 );

   assert( tap_buf != NULL );

   tap_buf_length = read( tap_f, tap_buf, 64 * 1024 );

   assert( tap_buf_length > 0 );

   gettimeofday( &tap_t1, NULL );
   //tap_buf = tap_buf.split( "\n" );
   line = strsep( &tap_buf, "\n" );
   while( line != NULL )
   {
      if( strlen( line ) > 0 )
      {
         tap_buf_arr[ lines++ ] = line;
      }
      line = strsep( &tap_buf, "\n" );
   }
};

void tap_pclose( void )
{
   pclose( fdopen( tap_f, "r" ) );
};

char* tap_badList;
int tap_testsRead, tap_bad, tap_good, tap_tests;

void tap_parse( void )
{
   int i = 0;
   char ch;
   char ibuf[ 64 ];
   tap_testsRead = 0;
   tap_bad = 0;
   tap_badList = GC_MALLOC( 4 * 1024 );
   assert( tap_badList != NULL );
   tap_badList[ 0 ] = ibuf[ 0 ] = '\0';
   tap_good = 0;
   tap_tests = -1;
   //tap_tests = parseInt( tap_buf[ 0 ].slice( 3 ), 10 );
   sscanf( tap_buf_arr[ 0 ], "1..%i", &tap_tests );

   if( tap_tests < 1 )
   {
      printf( "Bad TAP plan\n" );

      exit( -1 );
   }

   for( i = 1; i < lines; i++ )
   {
      ch = tap_buf_arr[ i ][ 0 ];

      switch( ch )
      {
         case( '#' ):
         case( '\r' ):
         case( '\n' ):
         case( ' ' ):
         case( '\t' ):
         {
            break;
         }
         default:
         {
            tap_testsRead++;

            if( strstr( tap_buf_arr[ i ], "not ok" ) == tap_buf_arr[ i ] )
            {
               sprintf( ibuf, "%i,", tap_testsRead );
               strcat( tap_badList, ibuf );
               tap_bad++;
            }
            else if( strstr( tap_buf_arr[ i ], "ok" ) != tap_buf_arr[ i ] )
            {
               sprintf( ibuf, "%i,", tap_testsRead );
               strcat( tap_badList, ibuf );
               tap_bad++;
            }
            else
            {
               tap_good++;
            }

            break;
         }
      }
   }
};

void tap_output( void )
{
   int t;
   timeval_subtract( &tap_t, &tap_t1, &tap_t0 );
   t = ( tap_t.tv_sec * 1000000 + tap_t.tv_usec ) / 1000;

   if( ( tap_tests == tap_testsRead ) &&
       ( tap_tests == tap_good ) )
   {
      printf( "%s .. ok\n", tap_fn );
      printf( "All tests successful.\n" );
      printf( "Files=1, Tests=%i, %i msecs\n", tap_testsRead, t );
      printf( "Result: PASS\n" );
   }
   else
   {
      printf( "%s .. %i subtests passed\n", tap_fn, tap_good );
      printf( "\n" );
      printf( "Test Summary Report\n" );
      printf( "-------------------\n" );
      printf( "%s (Tests: %i Failed: %i)\n", tap_fn, tap_testsRead,
              tap_bad );
      if( strlen( tap_badList ) > 0 )
      {
         tap_badList[ strlen( tap_badList ) - 2 ] = '\0';
         printf( "Failed tests: %s\n", tap_badList );
      }
      if( tap_tests != tap_testsRead )
      {
         printf( "Parse errors: Bad plan.  You planned %i tests but ran "
                 "%i.\n", tap_tests, tap_testsRead );
      }
      printf( "Files=1, Tests=%i, %i msecs\n", tap_testsRead, t );
      printf( "Result: FAIL\n" );
   }
};

char* tap_fn;

void tap_run( char* f )
{
   tap_fn = f;

   tap_popen( tap_fn );

   tap_pslurp();

   tap_pclose();

   tap_parse();

   tap_output();
};

int main( int argc, char** argv )
{
   GC_INIT();

   if( argc < 2 )
   {
      printf( "Missing an argument\n" );

      return( -1 );
   }

   tap_run( argv[ 1 ] );
}
