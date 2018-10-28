PREFIX ?= /usr/local

BIN = carp
LIB = lib$(BIN).a

CFLAGS  += -Isrc -Ideps -std=c11 -pedantic -Wall -W -Wextra \
	   -Wwrite-strings -Wpointer-arith -Wbad-function-cast \
	   -Wsign-conversion -Wconversion
LDFLAGS += -lm

MAIN_SRCS = src/main.c
MAIN_OBJS = $(MAIN_SRCS:.c=.o)

SRC_FILES = hashtable.c instructions.c lexer.c machine.c registers.c stack.c \
	    tokenizer.c file.c

LIB_SRCS = $(addprefix src/,$(SRC_FILES)) \
	   $(wildcard deps/talloc/*.c)
LIB_HDRS = $(wildcard src/carp/*.h) \
	   $(wildcard deps/talloc/*.h)
LIB_OBJS = $(LIB_SRCS:.c=.o)

TEST_SRCS = $(wildcard tests/*.c)
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_BINS = $(TEST_SRCS:.c=)

TEST_TARGETS = $(addprefix test_,$(TEST_BINS:tests/%=%))

TEST_X_SRCS = $(wildcard deps/console-colors/*.c) $(wildcard deps/test/*.c) \
	      $(wildcard deps/talloc/*.c)
TEST_X_OBJS = $(TEST_X_SRCS:.c=.o)

ifeq ($(RELEASE),yes)
	CFLAGS += -O2
	CFLAGS += -DNDEBUG
else
	CFLAGS  += -g3
	LDFLAGS += -g
endif

# TODO Get rid of this
ifeq ($(CDEBUG),yes)
	CFLAGS += -DCDEBUG
endif

.PHONY: all clean install uninstall test
all: $(BIN)

strip: $(BIN)
	strip -xs $(BIN)

$(BIN): $(MAIN_OBJS) $(LIB)
	$(CC) $(LDFLAGS) $^ -o $@

$(LIB): $(LIB_OBJS)
	ar crs $@ $^

tests/%: tests/%.o $(TEST_X_OBJS) $(LIB)
	$(CC) $(LDFLAGS) $^ -o $@

src/%.o: src/%.c $(LIB_HDRS)
tests/%.o: tests/%.c $(LIB_HDRS)
deps/console-colors/%.o: deps/console-colors/%.c
deps/test/%.o: deps/test/%.c
deps/talloc/%.o: deps/talloc/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:
test_%: tests/%
	@echo
	./$<

test: $(TEST_TARGETS)

clean:
	rm -rf $(MAIN_OBJS) \
	       $(BIN) \
	       $(LIB_OBJS) \
	       $(LIB) \
	       $(TEST_OBJS) \
	       $(TEST_X_OBJS) \
	       $(TEST_BINS)

install:
	test -d ${DESTDIR}${PREFIX}/bin || mkdir -p ${DESTDIR}${PREFIX}/bin
	test -d ${DESTDIR}${PREFIX}/lib || mkdir -p ${DESTDIR}${PREFIX}/lib
	test -d ${DESTDIR}${PREFIX}/include/carp || \
		mkdir -p ${DESTDIR}${PREFIX}/carp
	test -d ${DESTDIR}${PREFIX}/include/carp/lib || \
		mkdir -p $(DESTDIR)$(PREFIX)/include/carp/lib
	cp src/*.h $(DESTDIR)$(PREFIX)/include/carp
	cp $(LIB) $(DESTDIR)$(PREFIX)/lib
	install -pm 755 $(BIN) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -r $(DESTDIR)$(PREFIX)/include/carp
	rm $(DESTDIR)$(PREFIX)/lib/$(LIB)
	rm $(DESTDIR)$(PREFIX)/bin/$(BIN)
