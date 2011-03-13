
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
		spdatapickle.o spxmlpickle.o spjsonpickle.o sppbpickle.o \
		spxmlrpcpickle.o spdpbuiltin.o spdpbuiltinclass.o

TARGET =  libspdatapickle.so libspdatapickle.a \
		spxml2struct spxml2class testaddrbook testbenchmark

#--------------------------------------------------------------------

all: $(TARGET)

libspdatapickle.so: $(LIBOBJS)
	$(LINKER) $(SOFLAGS) $^ -o $@

libspdatapickle.a: $(LIBOBJS)
	$(AR) $@ $^

spxml2struct: spxml2struct.o spdpxml.o spdpsyntax.o spdpname.o spdpcode.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspdatapickle -o $@

spxml2class: spxml2class.o spdpxml.o spdpsyntax.o spdpname.o spdpclasscode.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspdatapickle -o $@

testaddrbook: dp_xyzaddrbookstruct.o testaddrbook.o dp_xyzaddrbookclass.o
	$(LINKER) $(LDFLAGS) $^ -static -L. -lspdatapickle -lspxml -lspjson -lstdc++ -lm -o $@

testbenchmark: dp_xyzbenchmarkstruct.o testbenchmark.o
	$(LINKER) $(LDFLAGS) $^ -static -L. -lspdatapickle -lspxml -lspjson -lstdc++ -lm -o $@

dp_xyzaddrbookstruct.o: dp_xyzaddrbookstruct.cpp

dp_xyzaddrbookstruct.cpp: addrbook.xml
	@(LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):../spxml/:../spjson; ./spxml2struct $^ .)

dp_xyzaddrbookclass.o: dp_xyzaddrbookclass.cpp

dp_xyzaddrbookclass.cpp: addrbook.xml
	@(LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):../spxml/:../spjson; ./spxml2class $^ .)

dp_xyzbenchmarkstruct.o: dp_xyzbenchmarkstruct.cpp

dp_xyzbenchmarkstruct.cpp: benchmark.xml
	@(LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):../spxml/:../spjson; ./spxml2struct $^ .)

clean:
	@( $(RM) *.o vgcore.* core core.* $(TARGET) dp_*.?pp )

#--------------------------------------------------------------------

# make rule
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@	

