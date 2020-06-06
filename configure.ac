AC_REVISION([m4_esyscmd_s([build-aux/git-version])])

AC_INIT([roq-logging],
        m4_esyscmd_s([build-aux/git-version]),
        [thraneh@gmail.com],
        [roq-logging])

AC_PREREQ([2.59])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_FILES([
 Makefile
 include/Makefile
 include/roq/Makefile
 src/Makefile
 src/roq/Makefile
 test/Makefile
])
AC_CONFIG_FILES([roq-logging.pc])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AM_PROG_AR
LT_INIT([disable-static])
AC_PROG_CXX
AC_LANG([C++])

AX_PTHREAD

ROQ_CHECK_STDCXX

ROQ_CHECK_FMT
ROQ_CHECK_GFLAGS
ROQ_CHECK_LIBUNWIND
ROQ_CHECK_SPDLOG

ROQ_CHECK_GTEST([third_party/googletest])

AC_OUTPUT
