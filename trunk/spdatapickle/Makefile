
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

CFLAGS += -I../
LDFLAGS += -L../spxml/ -lspxml -L../spjson/ -lspjson

LIBOBJS = spdpbase64.o spdpmetainfo.o spdpalloc.o spdpmetautils.o \
		spdatapickle.o spxmlpickle.o spjsonpickle.o sppbpickle.o

TARGET =  libspdatapickle.so libspdatapickle.a \
		spxml2struct testaddrbook

#--------------------------------------------------------------------

all: $(TARGET)

libspdatapickle.so: $(LIBOBJS)
	$(LINKER) $(SOFLAGS) $^ -o $@

libspdatapickle.a: $(LIBOBJS)
	$(AR) $@ $^

spxml2struct: spxml2struct.o spdpxml.o spdpsyntax.o spdpname.o spdpcode.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspdatapickle -o $@

testaddrbook: dp_xyzaddrbook.o testaddrbook.o
	$(LINKER) $(LDFLAGS) $^ -static -L. -lspdatapickle -lspxml -lspjson -lstdc++ -lm -o $@

dp_xyzaddrbook.o: dp_xyzaddrbook.cpp

dp_xyzaddrbook.cpp: addrbook.xml
	@(LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):../spxml/; ./spxml2struct addrbook.xml)

clean:
	@( $(RM) *.o vgcore.* core core.* $(TARGET) )

#--------------------------------------------------------------------

# make rule
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@	

