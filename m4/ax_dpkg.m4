AC_DEFUN([AX_DPKG],
[
    AC_PATH_PROG(DPKG_DEB, [dpkg-deb])
    AC_SUBST([DPKG_DEB])
    AM_CONDITIONAL([HAVE_DEBIAN_PACKAGING], [test x$DPKG_DEB != x])

    AS_IF([test x$DPKG_DEB = x], [AX_RED_WARN([dpkg-deb not found, disabled debian packaging])])
])
