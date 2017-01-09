AUTOMAKE_OPTIONS = foreign

SUBDIRS = 

bin_PROGRAMS = qatar
qatar_SOURCES = \
	Config.cc \
	Server.cc \
	Main.cc 

qatar_CPPFLAGS = -I$(top_srcdir) \
		$(BOOST_CPPFLAGS) \
		$(SOIL_CPPFLAGS) \
		$(RAPIDJSON_CPPFLAGS) \
		$(JSON_CPPFLAGS) \
		$(CPPDB_CPPFLAGS) \
		$(SUBJECT_CPPFLAGS) \
		$(ZOD_CPPFLAGS)

qatar_LDADD = \
	$(BOOST_LIBADD) \
	$(SOIL_LIBADD) \
	$(JSON_LIBADD) \
	$(CPPDB_LIBADD) \
	$(SUBJECT_LIBADD) \
	$(ZOD_LIBADD)


qatar_LDFLAGS = $(LIBADD_DL) -Wl,--no-as-needed -pthread

# header files
noinst_HEADERS = \
	Log.hh \
	Config.hh \
	Server.hh

EXTRA_DIST = env.sh qatar.cfg log.cfg

qatardir = $(bindir)
qatar_DATA = $(EXTRA_DIST)
