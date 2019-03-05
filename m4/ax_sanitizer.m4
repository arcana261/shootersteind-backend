AC_DEFUN([AX_SANITIZER],
[
    AX_CHECK_COMPILE_FLAG([-fsanitize=leak], [SANITY_LEAK=true], [SANITY_LEAK=false])
    AX_CHECK_COMPILE_FLAG([-fsanitize=address], [SANITY_ADDRESS=true], [SANITY_LEAK=false])
    AX_CHECK_COMPILE_FLAG([-fsanitize=undefined], [SANITY_UNDEFINED=true], [SANITY_UNDEFINED=false])
    AX_CHECK_COMPILE_FLAG([-fsanitize=memory], [SANITY_MEMORY=true], [SANITY_MEMORY=false])

    AX_CHECK_COMPILE_FLAG([-fsanitize=thread], [
        AC_MSG_CHECKING([whether thread sanitizer can be executed])

        cache_cflags=$[]_AC_LANG_PREFIX[]FLAGS
        _AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS -fsanitize=thread"

        AC_RUN_IFELSE([AC_LANG_PROGRAM([], [return 0;])], [
            SANITY_THREAD=true
            SANITY_THREAD_CFLAGS="-fsanitize=thread"
            SANITY_THREAD_LDFLAGS=""
        ], [
            cache_ldflags=$LDFLAGS
            LDFLAGS="$LDFLAGS -pie"

            AC_RUN_IFELSE([AC_LANG_PROGRAM([], [return 0;])], [
                SANITY_THREAD=true
                SANITY_THREAD_CFLAGS="-fsanitize=thread"
                SANITY_THREAD_LDFLAGS="-pie"
            ], [
                LDFLAGS="$LDFLAGS -ltsan"

                AC_RUN_IFELSE([AC_LANG_PROGRAM([], [return 0;])], [
                    SANITY_THREAD=true
                    SANITY_THREAD_CFLAGS="-fsanitize=thread"
                    SANITY_THREAD_LDFLAGS="-pie"
                ], [
                    SANITY_THREAD=false
                    SANITY_THREAD_CFLAGS=""
                    SANITY_THREAD_LDFLAGS=""
                ], [
                    SANITY_THREAD=false
                    SANITY_THREAD_CFLAGS=""
                    SANITY_THREAD_LDFLAGS=""
                ])
            ], [
                SANITY_THREAD=false
                SANITY_THREAD_CFLAGS=""
                SANITY_THREAD_LDFLAGS=""
            ])

            LDFLAGS=$cache_ldflags
        ], [
            SANITY_THREAD=false
            SANITY_THREAD_CFLAGS=""
            SANITY_THREAD_LDFLAGS=""
        ])

        _AC_LANG_PREFIX[]FLAGS=$cache_cflags

        AC_MSG_RESULT([$SANITY_THREAD])
    ], [
        SANITY_THREAD=false
        SANITY_THREAD_CFLAGS=""
        SANITY_THREAD_LDFLAGS=""
    ])

    AC_SUBST([SANITY_THREAD_CFLAGS])
    AC_SUBST([SANITY_THREAD_LDFLAGS])

    AM_CONDITIONAL([HAVE_LEAK_SANITIZER], [test x$SANITY_LEAK = xtrue])
    AM_CONDITIONAL([HAVE_ADDRESS_SANITIZER], [test x$SANITY_ADDRESS = xtrue])
    AM_CONDITIONAL([HAVE_UNDEFINED_SANITIZER], [test x$SANITY_UNDEFINED = xtrue])
    AM_CONDITIONAL([HAVE_THREAD_SANITIZER], [test x$SANITY_THREAD = xtrue])
    AM_CONDITIONAL([HAVE_MEMORY_SANITIZER], [test x$SANITY_MEMORY = xtrue])

    AS_IF([test x$SANITY_LEAK != xtrue], [AX_RED_WARN([disabled memory leak sanitizer test])])
    AS_IF([test x$SANITY_ADDRESS != xtrue], [AX_RED_WARN([disabled memory address sanitizer test])])
    AS_IF([test x$SANITY_UNDEFINED != xtrue], [AX_RED_WARN([disabled undefined behaviour sanitizer test])])
    AS_IF([test x$SANITY_THREAD != xtrue], [AX_RED_WARN([disabled thread sanitizer test])])
    AS_IF([test x$SANITY_MEMORY != xtrue], [AX_RED_WARN([disabled memory sanitizer test])])
])
