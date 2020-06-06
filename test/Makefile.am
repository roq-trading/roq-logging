# SO 35998856
check_LTLIBRARIES = libgtest.la
libgtest_la_SOURCES = \
	$(GTEST_DIR)/googletest/src/gtest-all.cc
libgtest_la_CPPFLAGS = \
	$(GTEST_CFLAGS)
libgtest_la_LDFLAGS = \
	$(PTHREAD_CFLAGS)

TESTS = roq-logging-test
check_PROGRAMS = roq-logging-test
roq_logging_test_LDADD = \
	$(PTHREAD_CFLAGS) \
	$(top_builddir)/src/roq/libroq-logging.la \
	$(SPDLOG_LIBS) \
	$(LIBUNWIND_LIBS) \
	$(GFLAGS_LIBS) \
	$(FMT_LIBS) \
	libgtest.la
roq_logging_test_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src \
	$(PTHREAD_CFLAGS) \
	$(GTEST_CFLAGS)
nodist_roq_logging_test_SOURCES = \
	main.cpp
