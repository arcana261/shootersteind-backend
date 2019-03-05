AC_DEFUN([AX_CLANG_FORMAT],
[
    AC_PATH_PROG([CLANG_FORMAT],[clang-format])
    AC_PATH_PROG([XARGS],[xargs])
    AC_PATH_PROG([TOOL_FIND],[find])
    AC_PATH_PROG([TOOL_CMP], [cmp])
    AC_SUBST([CLANG_FORMAT])
    AC_SUBST([XARGS])
    AC_SUBST([TOOL_FIND])
    AC_SUBST([TOOL_CMP])
    AM_CONDITIONAL([HAVE_CLANG_FORMAT], [test x$CLANG_FORMAT != x -a x$XARGS != x -a x$TOOL_FIND != x -a x$TOOL_CMP != x])

    AS_IF([test x$CLANG_FORMAT = x], [AX_RED_WARN([clang-format not found, disabled code reformatting])])
    AS_IF([test x$XARGS = x], [AX_RED_WARN([xargs not found, disabled code reformatting])])
    AS_IF([test x$TOOL_FIND = x], [AX_RED_WARN([find not found, disabled code reformatting])])
    AS_IF([test x$TOOL_CMP = x], [AX_RED_WARN([cmp not found, disabled code reformatting])])
])
