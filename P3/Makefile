CXX=clang++
CC=clang
LD=clang++
CXXFLAGS=$(CFLAGS) -std=c++11
CFLAGS=-Wall -Werror -Wextra -g
LDFLAGS=-g
RM=rm -f
LN=ln -f
MKDIR=mkdir -p
VPATH=src
OBJDIR=build
.PHONY=clean

CXXSOURCES=main.cpp huff.cpp lz1.cpp lz2.cpp codec.cpp tempFile.cpp
CSOURCES=
HEADERS=huff.h lz1.h lz2.h codec.h tempFile.h

CXXOBJECTS=$(CXXSOURCES:.cpp=.o)
COBJECTS=$(CSOURCES:.c=.o)

OBJECTS=$(COBJECTS) $(CXXOBJECTS)
EXECUTABLE=p3
LINKS=HUFF LZ1 LZ2 EXPAND
OTHER_FILES=gmon.out
.PHONY=test clean

all: $(OBJDIR) $(OBJDIR)/$(EXECUTABLE) $(LINKS)
#all: $(LINKS)
	
$(OBJDIR)/$(EXECUTABLE): $(patsubst %.o,$(OBJDIR)/%.o,$(OBJECTS))
	$(LD) $(LDFLAGS) -o $@ $^

#main.o: main.cpp mpz_test.cpp $(HEADERS)
#	$(CXX) $(CXXFLAGS) main.cpp -o $@
#timeTest.o: timeTest.cpp $(HEADERS)
#	$(CXX) -c -Wall -Werror -Wextra -g -pg -DNDEBUG -std=c++11 timeTest.cpp -o $@
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