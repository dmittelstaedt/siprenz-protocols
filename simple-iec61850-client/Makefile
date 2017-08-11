#Directories
BLDDIR=build
SRCDIR=src
INCDIR=include
LIBDIR=/usr/local/lib
INCDIR_GLOBAL=/usr/local/include/iec61850
INSDIR=/usr/local/bin

#Compiler
CC = gcc

#Compiler Flags
#CFLAGS += -std=gnu99
#CFLAGS += -Wstrict-prototypes
#CFLAGS += -Wsign-compare
#CFLAGS += -Wpointer-arith
#CFLAGS += -Wnested-externs
#CFLAGS += -Wmissing-declarations
#CFLAGS += -Wshadow
CFLAGS += -Wall
# -g create debugging information
CFLAGS += -g
CFLAGS += -fPIC
CFLAGS += -pie
CFLAGS += -rdynamic
CFLAGS += -U_FORTIFY_SOURCE

.PHONY: clean

build:
	mkdir -p $(BLDDIR)
#-L$(LIBDIR)
	$(CC) $(CFLAGS) $(SRCDIR)/simple-iec61850-client.c -I$(INCDIR_GLOBAL) -liec61850 -lpthread -o $(BLDDIR)/simple-iec61850-client

install:
	cp $(BLDDIR)/simple-iec61850-client $(INSDIR)/simple-iec61850-client

clean:
	rm -rf $(BLDDIR)

uninstall:
	rm -rf $(INSDIR)/simple-iec61850-client
