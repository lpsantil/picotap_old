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

var tap =
{
   "buf": "",
   "testnum": 0,
   "print": function print( s ){},
   "done": function done()
   {
      tap.t1 = new Date();
   },
   "plan": function plan( num )
   {
      tap.t0 = new Date();
      tap.total = num;
      tap.print( "1.." + num );
   },
   "ok": function ok( okay, msg )
   {
      tap.print( "" +
                 ( okay ? "ok " : "not ok " ) +
                 ++tap.testnum + " - " + msg );
   },
};

// If we're in some modern browser...
if( console )
{
   tap.print = function print( s )
   {
      console.log( s );
   };
}
else
{
// Otherwise, we're expecting some command line version of OneMonkey
   tap.print = function print_( s )
   {
      print( s + "\n" );
   }
}

