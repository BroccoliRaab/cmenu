SRC = cmenu.c
OBJ = cmenu.o 

PREFIX ?= /usr/local
CFLAGS += -std=c99 -pedantic -Wall -Werror -flto -O2
LDFLAGS += -flto -O2 
LDLIBS += -lncursesw

PROGRAM := cmenu
$(PROGRAM): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)  
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c -o $(OBJ) $(SRC)
clean:
	rm -f $(OBJ) $(PROGRAM)
install: $(PROGRAM)
	mkdir -p "$(PREFIX)/bin"
	cp cmenu "$(PREFIX)/bin/$(PROGRAM)"
uninstall:
	rm -f "$(PREFIX)/bin/$(PROGRAM)"
.PHONY:  install uninstall clean	  
