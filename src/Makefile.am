AUTOMAKE_OPTIONS = foreign

SUBDIRS = 

bin_PROGRAMS = qatar
qatar_SOURCES = \
	Options.cc \
	Server.cc \
	Main.cc 

qatar_CPPFLAGS = -I$(top_srcdir) \
		$(BOOST_CPPFLAGS) \
		$(SOIL_CPPFLAGS) \
		$(RAPIDJSON_CPPFLAGS) \
		$(CPPDB_CPPFLAGS) \
		$(RAPIDJSON_CPPFLAGS) \
		$(FMT_CPPFLAGS) \
		$(SPDLOG_CPPFLAGS) \
		$(CATA_CPPFLAGS)

qatar_LDADD = \
	$(BOOST_LIBADD) \
	$(SOIL_LIBADD) \
	$(FMT_LIBADD) \
	$(CPPDB_LIBADD) \
	$(CATA_LIBADD)

qatar_LDFLAGS = $(LIBADD_DL) -Wl,--no-as-needed -pthread

# header files
noinst_HEADERS = \
	Options.hh \
	Server.hh

EXTRA_DIST = env.sh qatar.json

qatardir = $(bindir)
qatar_DATA = $(EXTRA_DIST)
