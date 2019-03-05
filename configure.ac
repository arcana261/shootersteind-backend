AC_PREREQ([2.69])
AC_INIT([shootersteind], [0.1.0], [https://github.com/arcana261/shooterstein-backend/issues])
AC_SUBST([MAJOR_VERSION], [0])

# 1. Start with version information of ‘0:0:0’ for each libtool library.
# 2. Update the version information only immediately before a public release of your software. More frequent updates are unnecessary, and only guarantee that the current interface number gets larger faster.
# 3. If the library source code has changed at all since the last update, then increment revision (‘c:r:a’ becomes ‘c:r+1:a’).
# 4. If any interfaces have been added, removed, or changed since the last update, increment current, and set revision to 0.
# 5. If any interfaces have been added since the last public release, then increment age.
# 6. If any interfaces have been removed or changed since the last public release, then set age to 0.
AC_SUBST([LIBVERSION], [0:0:0])

# check file exists
# AC_CONFIG_SRCDIR([src/shootersteind/shootersteind.h])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.14 -Wall -Werror subdir-objects])

AM_PROG_AR
AC_PROG_CXX
AC_LANG([C++])
AX_COMPILER_CHECKS
AX_CHECK_CFLAG_APPEND([-std=gnu89])
AX_CHECK_CFLAG_APPEND([-Werror])
AX_CHECK_CFLAG_APPEND([-Wall])
AX_CHECK_CFLAG_APPEND([-fPIC])

LT_INIT

AC_PROG_INSTALL

# check availability of cpplint
AX_CPPLINT

# check availability of clang-format to format code
AX_CLANG_FORMAT

# check if we can perform unit tests
AX_UNITTEST

# check support for required compiler flags for sanity checkings
AX_SANITIZER

# Checks for existence of coverage tools and define variables for reporting coverage
AX_COVERAGE

# require boost
BOOST_REQUIRE([1.60])
BOOST_SYSTEM

# enable doxygen support
DX_DOXYGEN_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_MAN_FEATURE(ON)
DX_INIT_DOXYGEN($PACKAGE_NAME, [Doxyfile], [doc])

# check availability of debian packaging tools
AX_DPKG

# set output headerfile includedir
AC_SUBST([includedir], [$includedir/shootersteind])

# enable some more compiler warnings
# these warning might mess-up with other AC_LINK_IFELSE
# and AC_RUN_IFELSE directives
AX_CHECK_CFLAG_APPEND([-Wextra])
AX_CHECK_CFLAG_APPEND([-Wno-unused-parameter])
AX_CHECK_CFLAG_APPEND([-Wstrict-prototypes])

# outputs
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([Doxyfile])

# generate all files
AC_OUTPUT
