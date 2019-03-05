AC_DEFUN([AX_RED_WARN],
[
    echo -e "\e@<:@31m"
    AC_MSG_WARN([[$1]])
    echo -e "\e@<:@0m"
])
