PLATFORM?=HIS3521
TOPDIR:=$(word 1, $(subst /build/posix, ,$(shell pwd)))
INCDIR:=$(TOPDIR)/inc
CURLDIR:=$(TOPDIR)/inc/curl
SRCDIR:=$(TOPDIR)/src

ifeq ($(PLATFORM),LINUX) 
CROSS:=
CC:=$(CROSS)gcc
CXX:=$(CROSS)g++
AR:=$(CROSS)ar
STRIP:=$(CROSS)strip
LINK:=$(AR) rcs
RANLIB:=$(CROSS)ranlib
LIBDIR:=$(TOPDIR)/lib/linux
EMULATIONDVR_LIBDIR:=-pthread -L$(LIBDIR) -lcurl    
CPPFLAGS+=-Wall -g -O0 -fPIC    -I$(INCDIR) -I$(CURLDIR)
CFLAGS+=-Wall -g -O0 -fPIC    -I$(INCDIR) -I$(CURLDIR)
endif


ifeq ($(PLATFORM),HIS3521) 
CROSS:=arm-hisiv100nptl-linux-
CC:=$(CROSS)gcc
CXX:=$(CROSS)g++
AR:=$(CROSS)ar
STRIP:=$(CROSS)strip
LINK:=$(AR) rcs
RANLIB:=$(CROSS)ranlib
LIBDIR:=$(TOPDIR)/lib/his3521
EMULATIONDVR_LIBDIR:=-L$(LIBDIR) -lcurl    
CPPFLAGS+=-Wall -g -O0 -fPIC    -I$(INCDIR) -I$(CURLDIR)
CFLAGS+=-Wall -g -O0 -fPIC    -I$(INCDIR) -I$(CURLDIR)
endif


OBJS:=$(patsubst $(SRCDIR)/%.c, $(SRCDIR)/%.o, $(wildcard $(SRCDIR)/*.c))
OBJS+=$(patsubst $(SRCDIR)/%.cpp, $(SRCDIR)/%.o, $(wildcard $(SRCDIR)/*.cpp))

TARGET:=CurlDemo

.PHONY : clean all 

all: $(TARGET)

$(TARGET):$(OBJS)
	$(CXX) $(CPPFLAGS) -lpthread -lm -o $@ $^ $(EMULATIONDVR_LIBDIR) 
	rm -f *.o
	
clean:
	rm -f $(TARGET)
	rm -f $(OBJS)

