include Makefile.am.inc 
EXTRA_DIST = README.html README-WIN32.html config.h.win32 json-c.vcproj
SUBDIRS = .

noinst_LTLIBRARIES = libjson-c.la 

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = json-c.pc

libjson_cincludedir = $(includedir)/json-c
libjson_cinclude_HEADERS = \
	arraylist.h \
	bits.h \
	debug.h \
	json.h \
	json_config.h \
	json_c_version.h \
	json_inttypes.h \
	json_object.h \
	json_object_iterator.h \
	json_object_private.h \
	json_tokener.h \
	json_util.h \
	linkhash.h \
	printbuf.h \
	random_seed.h

libjson_c_la_SOURCES = \
	arraylist.c \
	debug.c \
	json_c_version.c \
	json_object.c \
	json_object_iterator.c \
	json_tokener.c \
	json_util.c \
	linkhash.c \
	printbuf.c \
	random_seed.c

