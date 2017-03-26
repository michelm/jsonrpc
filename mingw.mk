PREFIX  	?= .
CFLAGS  	:= -O3 -g -Wall -I$(PREFIX)/include -I.
LINKFLAGS 	:= -L$(PREFIX)/bin -ljansson-4
MAJOR   	:= 1
NAME    	:= jsonrpc
VERSION 	:= $(MAJOR)
TARGET  	:= lib$(NAME)-$(VERSION)
SOURCES 	:= jsonrpc.c
OBJECTS 	:= $(SOURCES:.c=.o)
SHLIB		:= $(TARGET).dll
STLIB		:= $(TARGET).a

shlib: $(SHLIB)

stlib: $(STLIB)

$(SHLIB): $(OBJECTS)
	$(CC) -shared -o $(TARGET).dll *.o -Wl,--out-implib,$(TARGET).a $(LINKFLAGS)

$(STLIB): $(OBJECTS)
	$(AR) rcs $@ $^

clean:
	$(RM) *.o *.a *.dll *.la *.so*

all: stlib shlib

install_shlib:
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/include
	cp *.h $(PREFIX)/include
	cp $(SHLIB) $(PREFIX)/bin

install_stlib:
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/include
	cp *.h $(PREFIX)/include
	cp $(STLIB) $(PREFIX)/lib

install: install_shlib install_stlib


