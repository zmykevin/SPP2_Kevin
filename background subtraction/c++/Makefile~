CXX ?= g++

CXXFLAGS += -c -std=c++11 -Wall -g $(shell pkg-config --cflags opencv)
LDFLAGS += $(shell pkg-config --libs --static opencv)

all: background_subtraction_main

background_subtraction_main: background_subtraction_main.o; $(CXX) $< -o $@ $(LDFLAGS)

%.o: %.cpp; $(CXX) $< -o $@ $(CXXFLAGS)

clean: ; rm -f background_subtraction_main.o background_subtraction_main test.o test
