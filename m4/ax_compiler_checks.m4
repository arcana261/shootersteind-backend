AC_DEFUN([AX_COMPILER_CHECKS],
[
    AC_MSG_CHECKING([for __LINE__ macro])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[const int x = __LINE__;]])],
        [
            AC_MSG_RESULT([ok])
        ],
        [
            AC_MSG_RESULT([no])
            AC_MSG_ERROR([sane __LINE__ macro is required])
        ])

    AC_MSG_CHECKING([for __FILE__ macro])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[const char *x = __FILE__;]])],
        [
            AC_MSG_RESULT([ok])
        ],
        [
            AC_MSG_RESULT([no])
            AC_MSG_ERROR([sane __FILE__ macro is required])
        ])

])