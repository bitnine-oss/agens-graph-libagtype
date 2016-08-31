SUBDIRS = json-c-0.12.1 test

AM_CFLAGS = -Ijson-c-0.12.1

include_HEADERS = agtype.h

lib_LTLIBRARIES = libagtype.la
libagtype_la_SOURCES = \
	agtype.c \
	agtype.h \
	tokenizer.c \
	tokenizer.h \
	ag_value.c

libagtype_la_LIBADD = json-c-0.12.1/libjson-c.la

libagtype_la_LDFLAGS = -version-info 1:0:0

