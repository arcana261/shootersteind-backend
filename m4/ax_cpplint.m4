AC_DEFUN([AX_CPPLINT],
[
    AC_PATH_PROG([CPPLINT],[cpplint])
    AC_PATH_PROG([XARGS],[xargs])
    AC_PATH_PROG([TOOL_FIND],[find])
    AC_SUBST([CPPLINT])
    AC_SUBST([XARGS])
    AC_SUBST([TOOL_FIND])
    AM_CONDITIONAL([HAVE_CPPLINT], [test x$CPPLINT != x -a x$XARGS != x -a x$TOOL_FIND != x])

    AS_IF([test x$CPPLINT = x], [AX_RED_WARN([cpplint not found, disabled linter support])])
    AS_IF([test x$XARGS = x], [AX_RED_WARN([xargs not found, disabled linter support])])
    AS_IF([test x$TOOL_FIND = x], [AX_RED_WARN([find not found, disabled linter support])])
])
