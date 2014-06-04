CXX=g++
CC=gcc
LD=g++
CXXFLAGS=$(CFLAGS) -std=c++11
CFLAGS=-Wall -Wextra -O3 -Werror -funroll-loops -fomit-frame-pointer -pipe -march=native #-g
LDFLAGS=-g
RM=rm -f
LN=ln -f
MKDIR=mkdir -p
VPATH=src
OBJDIR=build
.PHONY=clean

CXXSOURCES=main.cpp huff.cpp codec.cpp tempFile.cpp lz.cpp
CSOURCES=
HEADERS=huff.h lz1.h lz2.h codec.h tempFile.h circularCounter.h lz.h

CXXOBJECTS=$(CXXSOURCES:.cpp=.o)
COBJECTS=$(CSOURCES:.c=.o)

OBJECTS=$(COBJECTS) $(CXXOBJECTS)
EXECUTABLE=p3
LINKS=HUFF LZ1 LZ2 EXPAND
OTHER_FILES=gmon.out
.PHONY=test clean

all: $(OBJDIR) $(OBJDIR)/$(EXECUTABLE) $(LINKS)
	
$(OBJDIR)/$(EXECUTABLE): $(patsubst %.o,$(OBJDIR)/%.o,$(OBJECTS))
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) -o $(OBJDIR)/$@ $^

$(LINKS): $(OBJDIR)/$(EXECUTABLE)
	$(LN) -s $(OBJDIR)/$(EXECUTABLE) $@

$(OBJDIR) : 
	$(MKDIR) $@

clean :
	$(RM) -r $(OBJDIR) $(LINKS) $(OTHER_FILES);
