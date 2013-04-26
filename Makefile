# Copyright (c) 2013, Louis P. Santillan <lpsantil@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Pull in platform specific settings.
#include Makefile.$(firstword $(subst _, ,$(shell uname -s)))

# Start off with a(n) (un)known state
OS = UNKNOWN
ARCH = UNKNOWN
ARCH_M = UNKNOWN
UNAME_M = UNKNOWN
UNAME_P = UNKNOWN
PREMAKE =
UID = $(shell id -u)

######################################################################

# Core count
ifndef CORES
	CORES = 1
endif

# Basic feature detection
OS = $(shell uname)
UNAME_M = $(shell uname -m)
UNAME_P = $(shell uname -p)
ARCH = $(shell arch)

# Install prefix
ifndef PREFIX
        PREFIX = /usr/local
endif

######################################################################

# Setup ARCH_M based on what we see
ifeq ($(ARCH),x86_64)
	ARCH_M = -m64
endif
ifeq ($(UNAME_M),x86_64)
	ARCH_M = -m64
endif
ifeq ($(ARCH),i386)
	ARCH_M = -m32
endif
ifeq ($(ARCH),i686)
	ARCH_M = -m32
endif
ifeq ($(ARCH),arm)
	ARCH_M =
endif


######################################################################

CC = cc
CFLAGS = -O3
SDIR = .
SWDEPS = ./Deps
APPS = ./Applications
TOOLS = ./Tools
TESTS = ./Tests
EXAMPLES = ./Examples
EXDIR = $(EXAMPLES)/C
IDIR = -I./
LDIR = -L./
PROG = tap
LIB = libtap

######################################################################

SRC = $(SDIR)/libtap.c

######################################################################

MAKE_ME_A_SANDWICH =
MAKE_ME_A_SANDWICH_ID = $(shell id -u)
ifeq ($(MAKECMDGOALS),me a sandwich)
	ifeq ($(MAKE_ME_A_SANDWICH_ID),0)
		MAKE_ME_A_SANDWICH=Okay.
	else
		MAKE_ME_A_SANDWICH=What? Make it yourself.
	endif
endif

######################################################################

all: lib examples $(PROG)

$(LIB): tap.h libtap.c
	$(CC) $(CFLAGS) -c libtap.c -o $(LIB).o

examples: tap.h $(LIB)
	$(CC) $(CFLAGS) $(EXDIR)/test-exit.c $(LIB).o -o $(EXDIR)/test-exit.exe
	$(CC) $(CFLAGS) $(EXDIR)/test-fail.c $(LIB).o -o $(EXDIR)/test-fail.exe
	$(CC) $(CFLAGS) $(EXDIR)/test-input.c $(LIB).o -o $(EXDIR)/test-input.exe
	$(CC) $(CFLAGS) $(EXDIR)/test-over.c $(LIB).o -o $(EXDIR)/test-over.exe
	$(CC) $(CFLAGS) $(EXDIR)/test-under.c $(LIB).o -o $(EXDIR)/test-under.exe
	$(CC) $(CFLAGS) $(EXDIR)/test.c $(LIB).o -o $(EXDIR)/test.exe

$(PROG): $(LIB)
	$(CC) $(CFLAGS) tap.c -o $(PROG)

install: install-libtap install-tap

install-libtap: tap.h $(LIB)
	cp libtap.o $(PREFIX)/lib/
	cp tap.h $(PREFIX)/include/

install-tap: $(LIB) $(PROG)
	cp tap $(PREFIX)/bin/

clean:
	rm -f $(PROG) *.o *.a

superclean: clean
	rm -f Examples/C/*.exe

ultraclean: superclean

showconfig:
	@echo picotap Build Config
	@echo "  "Feature Detection
	@echo "  ""  "OS = $(OS)
	@echo "  ""  "ARCH = $(ARCH)
	@echo "  ""  "UNAME_M = $(UNAME_M)
	@echo "  ""  "UNAME_P = $(UNAME_P)
	@echo "  ""  "CORES = $(CORES)
	@echo "  ""  "UID = $(UID)
	@echo "  "Feature Detection Results
	@echo "  ""  "ARCH_M = $(ARCH_M)
	@echo "  ""  "CFLAGS = $(CFLAGS)
	@echo "  ""  "LIBS = $(LIBS)
	@echo "  ""  "PREMAKE = $(PREMAKE)
	@echo "  "Build Settings "(Change by modifying Makfile)"
	@echo "  ""  "CC = $(CC)
	@echo "  ""  "PROG = $(PROG)

help:
	@echo picotap Build methods
	@echo "  "all: 
	@echo "  "clean: Remove tap, libtap and picotap object files
	@echo "  "superclean: clean + remove examples
	@echo "  "ultraclean: superclean + tests
	@echo "  "
	@echo Build from scratch:
	@echo "  "make

me:
	@echo >/dev/null
a:
	@echo >/dev/null
sandwich:
	@echo $(MAKE_ME_A_SANDWICH)

