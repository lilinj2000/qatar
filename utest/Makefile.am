AUTOMAKE_OPTIONS = foreign

# TESTS = unittest.sh
check_PROGRAMS = unittest

unittest_SOURCES = \
	../src/DBService.cc \
	DBServiceTest.cc

unittest_CPPFLAGS = -I$(top_srcdir) \
		$(BOOST_CPPFLAGS) \
		$(SOIL_CPPFLAGS) \
		$(RAPIDJSON_CPPFLAGS) \
		$(CPPDB_CPPFLAGS) \
		$(RAPIDJSON_CPPFLAGS) \
		$(FMT_CPPFLAGS) \
		$(SPDLOG_CPPFLAGS) \
		$(READERWRITERQUEUE_CPPFLAGS) \
		$(GTEST_CPPFLAGS)

unittest_LDADD = \
	$(GTEST_LIBADD) \
	$(BOOST_LIBADD) \
	$(SOIL_LIBADD) \
	$(FMT_LIBADD) \
	$(CPPDB_LIBADD)

unittest_LDFLAGS = $(LIBADD_DL) -Wl,--no-as-needed -pthread -lrt

# header files
noinst_HEADERS = 

EXTRA_DIST = env.sh
