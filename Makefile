CXX=g++
CFLAGS=-c -Wall -std=c++0x
LDFLAGS=-lSDL -lSDL_image
EXECUTABLE=MX
SOURCES=main.cpp application.cpp debug.cpp tilerenderer.cpp sdlgraphics.cpp sdlsurface.cpp map.cpp
OBJECTS=$(SOURCES:.cpp=.o)

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS+=-D_DEBUG -g
else
	CFLAGS+=-O3
endif

all: $(SOURCES) executable

executable: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
