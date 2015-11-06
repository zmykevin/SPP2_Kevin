CXX ?= g++

CXXFLAGS += -c -Wall $(shell pkg-config --cflags opencv)
LDFLAGS += $(shell pkg-config --libs --static opencv)

all: background_subtraction_main test

background_subtraction_main: background_subtraction_main.o; $(CXX) $< -o $@ $(LDFLAGS)

test: test.o; $(CXX) $< -o $@ $(LDFLAGS)

%.o: %.cpp; $(CXX) $< -o $@ $(CXXFLAGS)

clean: ; rm -f background_subtraction_main.o background_subtraction_main test.o test
