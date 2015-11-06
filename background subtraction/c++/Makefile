CC = g++

CFLAGS = -c -Wall 'pkh-config --cflags opencv'
LIBS = `pkg-config --libs opencv`

all: background_subtraction_main

background_subtraction_main: background_subtraction_main.o; $(CXX) $< -o $@ $(LDFLAGS)

%.o: %.cpp; $(CXX) $< -o $@ $(CXXFLAGS)

clean: ; rm -f background_subtraction_main.o background_subtraction_main
