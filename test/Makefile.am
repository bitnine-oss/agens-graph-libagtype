AM_CFLAGS = -I../json-c-0.12.1 -I../

TEST_SRCS = test_ag_json.c \
			test_graph.c \
			test_tokenizer.c \
			test_odbc.c

check_PROGRAMS = agtest
agtest_SOURCES = AllTests.c \
				 CuTest.c \
				 Cutest.h \
				 $(TEST_SRCS)

AllTests.c: $(TEST_SRCS)
	./make-tests.sh $^ > $@

agtest_LDADD = ../libagtype.la ../libagodbc-type.la -lodbc

clean-local:
	-rm -rf AllTests.c
