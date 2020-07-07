lib_LTLIBRARIES = libroq-logging.la
libroq_logging_la_LDFLAGS = \
	-avoid-version
libroq_logging_la_LIBADD = \
	$(SPDLOG_LIBS) \
	$(LIBUNWIND_LIBS) \
	$(GFLAGS_LIBS) \
	$(FMT_LIBS) \
	$(ABSEIL_LIBS)
libroq_logging_la_CPPFLAGS = \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/src \
	$(ABSEIL_CFLAGS) \
	$(FMT_CFLAGS) \
	$(GFLAGS_CFLAGS) \
	$(LIBUNWIND_CFLAGS) \
	$(SPDLOG_CFLAGS)
libroq_logging_la_SOURCES = \
	application.cpp \
	logging.cpp \
	unwind.cpp
