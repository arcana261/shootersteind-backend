ACLOCAL_AMFLAGS = -I m4 --install
AM_CPPFLAGS = -I$(top_srcdir)/src/shootersteind -I$(top_srcdir)/src -I$(top_srcdir)/test
AM_CFLAGS = 
AM_LDFLAGS = 

EXTRA_DIST = \
	autogen.sh \
	CPPLINT.cfg \
	Doxyfile \
	test/testing.h

# include libuv as dependency
# AM_CFLAGS += $(LIBUV_CFLAGS) 
# AM_LDFLAGS += $(LIBUV_LIBS)

# targets
bin_PROGRAMS = shootersteind

# libuvchan-0.la
shootersteind_SOURCES = \
	src/shootersteind/main.cpp
shootersteind_LDFLAGS = $(AM_LDFLAGS)

# tests
check_PROGRAMS = \
	test/shootersteind/sample_test

# test/shootersteind/sample
test_shootersteind_sample_test_SOURCES = test/shootersteind/sample_test.c
test_shootersteind_sample_test_LDADD = $(lib_LTLIBRARIES)

# makefile includes
include make/lint.am
include make/format.am
include make/check.am
include make/sanity.am
include make/coverage.am
include make/doxygen.am
include make/docs.am
include make/clean.am
include make/installcheck-lib.am
include make/phony.am
