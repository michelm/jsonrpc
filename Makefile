PREFIX  ?= .
CFLAGS  := -fPIC -O3 -g -Wall -Werror -I$(PREFIX)/include
MAJOR   := 0
MINOR   := 1
NAME    := jsonrpc
VERSION := $(MAJOR).$(MINOR)
SOURCES := jsonrpc.c jsonrpc_notify.c
OBJECTS := $(SOURCES:.c=.o)
TESTBIN := jsonrpc_test

lib: lib$(NAME).so.$(VERSION)

all: lib

lib$(NAME).so.$(VERSION): $(OBJECTS)
	$(CC) -shared -Wl,-soname,lib$(NAME).so.$(MAJOR) $^ -o $@

clean:
	$(RM) *.o *.so*

test:
	$(CC) $(SOURCES) jsonrpc_server.c -o $(TESTBIN) -lzmq -ljansson
	mkdir -p $(PREFIX)/bin
	cp $(TESTBIN) $(PREFIX)/bin

install: all
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	ln -srf lib$(NAME).so.$(VERSION) lib$(NAME).so.0 
	ln -srf lib$(NAME).so.$(VERSION) lib$(NAME).so
	cp lib$(NAME).so.$(VERSION) $(PREFIX)/lib
	mv lib$(NAME).so.0 $(PREFIX)/lib
	mv lib$(NAME).so $(PREFIX)/lib
	cp *.h $(PREFIX)/include

