
#--------------------------------------------------------------------

CC = gcc
AR = ar cru
CFLAGS = -Wall -D_REENTRANT -D_GNU_SOURCE -g -fPIC
SOFLAGS = -shared
LDFLAGS = -lstdc++

LINKER = $(CC)
LINT = lint -c
RM = /bin/rm -f

#--------------------------------------------------------------------

CFLAGS += -I../spxml/
LDFLAGS += -L../spxml/ -lspxml

LIBOBJS = spdpbase64.o spdpmetainfo.o spdpalloc.o spdpmetautils.o spxmlpickle.o

TARGET =  libspdatapickle.so \
		spxml2struct testaddrbook

#--------------------------------------------------------------------

all: $(TARGET)

libspdatapickle.so: $(LIBOBJS)
	$(LINKER) $(SOFLAGS) $^ -o $@

spxml2struct: spxml2struct.o spdpxml.o spdpsyntax.o spdpname.o spdpcode.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspdatapickle -o $@

testaddrbook: xyzaddrbook.o testaddrbook.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspdatapickle -o $@

xyzaddrbook.o: xyzaddrbook.cpp

xyzaddrbook.cpp: addrbook.xml
	@(LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):../spxml/; ./spxml2struct addrbook.xml)

clean:
	@( $(RM) *.o vgcore.* core core.* $(TARGET) )

#--------------------------------------------------------------------

# make rule
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@	

