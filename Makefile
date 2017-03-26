PREFIX  ?= .
CFLAGS  := -fPIC -O3 -g -Wall -Werror -I$(PREFIX)/include -I.
MAJOR   := 1
MINOR   := 2
NAME    := jsonrpc
VERSION := $(MAJOR).$(MINOR)
SOURCES := jsonrpc.c
OBJECTS := $(SOURCES:.c=.o)
TESTBIN := jsonrpc_test
SHLIB	:= lib$(NAME).so.$(VERSION)
STLIB	:= lib$(NAME).a

shlib: $(SHLIB)

stlib: $(STLIB)

$(SHLIB): $(OBJECTS)
	$(CC) -shared -Wl,-soname,lib$(NAME).so.$(MAJOR) $^ -o $@

$(STLIB): $(OBJECTS)
	$(AR) rcs $@ $^

clean:
	$(RM) *.o *.so* *.la *.a *.dll

test:
	$(CC) $(SOURCES) jsonrpc_server.c -o $(TESTBIN) -lzmq -ljansson
	mkdir -p $(PREFIX)/bin
	cp $(TESTBIN) $(PREFIX)/bin

all: shlib stlib

install_shlib: shlib
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	cp *.h $(PREFIX)/include
	ln -srf $(SHLIB) lib$(NAME).so.$(MAJOR)
	ln -srf $(SHLIB) lib$(NAME).so
	cp $(SHLIB) $(PREFIX)/lib
	mv lib$(NAME).so.$(MAJOR) $(PREFIX)/lib
	mv lib$(NAME).so $(PREFIX)/lib

install_stlib: stlib
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	cp *.h $(PREFIX)/include
	cp $(STLIB) $(PREFIX)/lib

install: install_shlib install_stlib
	

