SUBDIRS = json-c-0.12.1 test

AM_CFLAGS = -Ijson-c-0.12.1

include_HEADERS = agtype.h agodbc-type.h

lib_LTLIBRARIES = libagtype.la libagodbc-type.la
libagtype_la_SOURCES = \
	agtype.h \
	tokenizer.c \
	tokenizer.h \
	ag_graph.c \
	ag_json.c

libagtype_la_LIBADD = json-c-0.12.1/libjson-c.la

libagtype_la_LDFLAGS = -version-info 1:0:0

libagodbc_type_la_SOURCES = ag_odbc.c

libagodbc_type_la_LIBADD = libagtype.la

libagodbc_type_la_LDFLAGS = -version-info 1:0:0
