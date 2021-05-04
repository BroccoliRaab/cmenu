OBJS := $(wildcard *.c) 
OBJS := $(OBJS:.c=.o)   

PREFIX ?= /usr/local
CFLAGS += -pedantic -Wall -flto -O2
LDFLAGS += -flto -O2 
LDLIBS += -lncurses

PROGRAM := cmenu

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<
$(PROGRAM): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)  
clean:
	rm -f $(OBJS) $(PROGRAM)
install: cmenu
	mkdir -p "$(PREFIX)/bin"
	cp cmenu "$(PREFIX)/bin/$(PROGRAM)"
uninstall:
	rm -f "$(PREFIX)/bin/$(PROGRAM)"
.PHONY:  install uninstall clean	  
