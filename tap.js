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

// In the browser, simulate reading stdin by making tap.print write to a
// buffer and having done perform the conversion of buffer to lines
if( console )
{
   tap.print = function print( s )
   {
      if( tap.buf == "" )
      {
         tap.buf = s;
      }
      else
      {
         tap.buf += "\n" + s;
      }
   };

   tap.done = function done()
   {
      tap.t1 = new Date();
      tap.buf = tap.buf.split( "\n" );
   };

   tap.popen = function popen( f ){};

   tap.pslurp = function pslurp(){};

   tap.pclose = function pclose(){};

   tap.log = function log( s )
   {
      console.log( s );
   };

   tap.doc = document.URL.split( "/" ).slice( -1 )[ 0 ];
}
else
{
   tap.popen = function popen( f )
   {
      tap.f = __._popen( f, "r" );

      if( tap.f < 0 )
      {
         throw( "Bad executable file: " + f );
      }

      tap.buf = "";
      tap.t0 = new Date();
   };

   tap.pslurp = function pslurp()
   {
      var chars = __._reads( 1024, tap.f );

      while( chars.length > 0 )
      {
         tap.buf += chars;
         chars = __._reads( 1024, tap.f );
      }

      tap.t1 = new Date();
      tap.buf = tap.buf.split( "\n" );
   };

   tap.pclose = function pclose()
   {
      __._pclose( tap.f );
   };

   tap.log = print;
}

tap.parse = function parse()
{
   var i = 0;
   tap.testsRead = 0;
   tap.bad = 0;
   tap.badList = "";
   tap.good = 0;
   tap.badInput = false;
   tap.tests = parseInt( tap.buf[ 0 ].slice( 3 ), 10 );

   if( isNaN( tap.tests ) )
   {
      throw( "Bad TAP plan" );
   }

   for( i = 1; i < tap.buf.length; i++ )
   {
      var ch = tap.buf[ i ].charAt( 0 );

      switch( ch )
      {
         case( "#" ):
         case( "\r" ):
         case( "\n" ):
         case( " " ):
         case( "\s" ):
         {
            break;
         }
         default:
         {
            tap.testsRead++;

            if( tap.buf[ i ].indexOf( "not ok" ) == 0 )
            {
               tap.badList += tap.testsRead + ",";
               tap.bad++;
            }
            else if( tap.buf[ i ].indexOf( "ok" ) != 0 )
            {
               tap.badList += tap.testsRead + ",";
               tap.badInput = true;
               tap.bad++;
            }
            else
            {
               tap.good++;
            }

            break;
         }
      }
   }
};

tap.output = function output()
{
   var t = tap.t1.getTime() - tap.t0.getTime();

   if( ( tap.tests === tap.testsRead ) &&
       ( tap.tests === tap.good ) )
   {
      tap.log( tap.fn + " .. ok" );
      tap.log( "All tests successful." );
      tap.log( "Files=1, Tests=" + tap.testsRead + ", " + t +
               " msecs" );
      tap.log( "Result: PASS" );
   }
   else
   {
      tap.log( tap.fn + " .. " + tap.good + " subtests passed" );
      tap.log( "" );
      tap.log( "Test Summary Report" );
      tap.log( "-------------------" );
      tap.log( tap.fn + " (Tests: " + tap.testsRead + " Failed: " +
               tap.bad + ")" );
      if( tap.badInput )
      {
         tap.log( "Bad TAP Input." );
      }
      if( tap.bad.length > 0 )
      {
         tap.log( "Failed tests: " + tap.badList.slice( 0, -1 ) );
      }
      if( tap.tests != tap.testsRead )
      {
         tap.log( "Parse errors: Bad plan.  You planned " +
                  tap.tests + " tests but ran " + tap.testsRead +
                  "." );
      }
      tap.log( "Files=1, Tests=" + tap.testsRead + ", " + t +
               " msecs" );
      tap.log( "Result: FAIL" );
   }
};

tap.run = function run( f )
{
   tap.fn = f;

   tap.popen( tap.fn );

   tap.pslurp();

   tap.pclose();

   tap.parse();

   tap.output();
};

function main( args )
{
   if( args.length < 2 )
   {
      throw( "Missing an argument" );
   }

   tap.run( args[ 1 ] );
}

