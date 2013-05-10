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

#include "taptime.h"

#ifdef __WATCOMC__
#include <sys/timeb.h>

/* <= POSIX 2001 */
int time_sub( TAP_time *result,
              TAP_time *x,
              TAP_time *y )
{
   long rt;
   rt = ( x->time * 1000 + x->millitm ) -
        ( y->time * 1000 + y->millitm );

   result->millitm = rt % 1000;
   result->time    = rt / 1000;

   /* Return 1 if result is negative. */
   return( rt < 0 );
}

#else
#include <sys/types.h>
#include <sys/time.h>

/* >= POSIX 2008 */
int time_sub( TAP_time *result,
              TAP_time *x,
              TAP_time *y )
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

#endif
