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
#include <assert.h>
#include "taptime.h"

#ifdef __WATCOMC__
extern FILE* popen( char* f, char* m );
extern int pclose( FILE* f );
#endif

TAP_time tap_t0, tap_t1, tap_t;

FILE* tap_f = NULL;
int tap_length;

void tap_popen( char* f )
{
   tap_f = popen( f, "r" );

   assert( tap_f != NULL );

   tap_length = 0;
};

char* tap_fn;

void tap_pclose( void )
{
   getTAPtime( &tap_t1 );
   pclose( tap_f );
};

char tap_badList[ 512 ];
int tap_testsRead, tap_bad, tap_good, tap_tests;

void tap_parse( void )
{
   char ch;
   char* rt;
   char ibuf[ 256 ];

   tap_testsRead = 0;
   tap_bad = 0;
   tap_badList[ 0 ] = ibuf[ 0 ] = '\0';
   tap_good = 0;
   tap_tests = -1;

   getTAPtime( &tap_t0 );
   rt = fgets( ibuf, 256, tap_f );
   assert( rt != NULL );
   sscanf( ibuf, "1..%i", &tap_tests );

   if( tap_tests < 1 )
   {
      printf( "Bad TAP plan\n" );

      exit( -1 );
   }

   while( fgets( ibuf, 256, tap_f ) != NULL )
   {
      ch = ibuf[ 0 ];

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

            if( strstr( ibuf, "not ok" ) == ibuf )
            {
               sprintf( ibuf, "%i,", tap_testsRead );
               strcat( tap_badList, ibuf );
               tap_bad++;
            }
            else if( strstr( ibuf, "ok" ) != ibuf )
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
   unsigned long t;

   time_sub( &tap_t, &tap_t1, &tap_t0 );

#ifdef __WATCOMC__
   t = tap_t.time * 1000 + tap_t.millitm;
#else
   t = ( tap_t.tv_sec * 1000000 + tap_t.tv_usec ) / 1000;
#endif

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

void tap_run( char* f )
{
   tap_fn = f;

   tap_popen( tap_fn );

   tap_parse();

   tap_pclose();

   tap_output();
};

int main( int argc, char** argv )
{
   if( argc < 2 )
   {
      printf( "Missing an argument\n" );

      return( -1 );
   }

   tap_run( argv[ 1 ] );

   return( 0 );
}
