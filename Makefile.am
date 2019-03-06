ACLOCAL_AMFLAGS = -I m4 --install
AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I$(top_srcdir)/src/shootersteind -I$(top_srcdir)/src -I$(top_srcdir)/test
AM_CFLAGS = 
AM_LDFLAGS = 

EXTRA_DIST = \
	autogen.sh \
	CPPLINT.cfg \
	Doxyfile \
	test/testing.h

# include libuv as dependency
AM_CFLAGS += $(PTHREAD_CFLAGS) 
AM_CPPFLAGS += $(PTHREAD_CFLAGS) 
AM_LDFLAGS += $(PTHREAD_LIBS) $(BOOST_SYSTEM_LDFLAGS)

# targets
bin_PROGRAMS = shootersteind

# libuvchan-0.la
shootersteind_SOURCES = \
	src/shootersteind/main.cpp \
	src/shootersteind/client.cpp \
	src/shootersteind/routes/routes.cpp \
	src/shootersteind/routes/login.cpp \
	src/shootersteind/routes/game.cpp \
	src/shootersteind/handlers/login.cpp \
	src/shootersteind/handlers/game.cpp
shootersteind_LDFLAGS = $(AM_LDFLAGS) \
	$(PTHREAD_LDFLAGS) $(PTHREAD_CFLAGS) \
	$(BOOST_SYSTEM_LDFLAGS) $(BOOST_SYSTEM_LIBS)

# tests
check_PROGRAMS = \
	test/shootersteind/queue_test \
	test/shootersteind/crow_test

# test/shootersteind/sample
test_shootersteind_queue_test_SOURCES = test/shootersteind/queue_test.cpp
test_shootersteind_queue_test_LDADD = $(PTHREAD_LDFLAGS) $(PTHREAD_CFLAGS)

test_shootersteind_crow_test_SOURCES = test/shootersteind/crow_test.cpp
test_shootersteind_crow_test_LDADD = \
	$(PTHREAD_LDFLAGS) $(PTHREAD_CFLAGS) \
	$(BOOST_SYSTEM_LDFLAGS) $(BOOST_SYSTEM_LIBS)

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
