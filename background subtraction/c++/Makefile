CXX ?= g++

CXXFLAGS += -c -std=c++11 -Wall -g $(shell pkg-config --cflags opencv)
LDFLAGS += $(shell pkg-config --libs --static opencv)

all: background_subtraction_main background_subtraction_main_v2 MOG_function test


background_subtraction_main: background_subtraction_main.o; $(CXX) $< -o $@ $(LDFLAGS)

background_subtraction_main_v2: background_subtraction_main_v2.o; $(CXX) $< -o $@ $(LDFLAGS)
%.o: %.cpp; $(CXX) $< -o $@ $(CXXFLAGS)

MOG_function: MOG_function.o; $(CXX) $< -o $@ $(LDFLAGS)
%.o: %.cpp; $(CXX) $< -o $@ $(CXXFLAGS)

test: test.o; $(CXX) $< -o $@ $(LDFLAGS)
%.o: %.cpp; $(CXX) $< -o $@ $(CXXFLAGS)

clean: ; rm -f background_subtraction_main.o background_subtraction_main background_subtraction_main_v2.o  background_subtraction_main_v2 MOG_function.o MOG_function test.o test
