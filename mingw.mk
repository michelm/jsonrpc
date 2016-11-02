PREFIX  	?= .
CFLAGS  	:= -fPIC -O3 -g -Wall -Werror -I$(PREFIX)/include -I.
LINKFLAGS 	:= -L$(PREFIX)/bin -ljansson-4
MAJOR   	:= 1
NAME    	:= jsonrpc
VERSION 	:= $(MAJOR)
TARGET  	:= lib$(NAME)-$(VERSION)

all: clean
	$(CC) -c jsonrpc.c $(CFLAGS)
	$(CC) -c jsonrpc_notify.c $(CFLAGS)
	$(CC) -shared -o $(TARGET).dll *.o -Wl,--out-implib,$(TARGET).a $(LINKFLAGS)

clean:
	$(RM) *.o *.a *.dll *.la *.so*
	
install: all
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/include
	cp $(TARGET).dll $(PREFIX)/bin
	cp *.h $(PREFIX)/include

