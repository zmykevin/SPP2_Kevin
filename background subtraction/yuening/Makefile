CXX ?= g++

CXXFLAGS += -c -std=c++11 -Wall -g $(shell pkg-config --cflags opencv)
LDFLAGS += $(shell pkg-config --libs --static opencv)

all: bouncing bg MOG_function
# all: detect_blob

detect_blob: detect_blob.o; $(CXX) $< -o $@ $(LDFLAGS)

main: main.o; $(CXX) $< -o $@ $(LDFLAGS)

bouncing: bouncing.o; $(CXX) $< -o $@ $(LDFLAGS)

bg: bg.o ball.o
	$(CXX) ball.o $< -o $@ $(LDFLAGS) 

test: test.o; $(CXX) $< -o $@ $(LDFLAGS)

MOG_function: MOG_function.o ball.o; $(CXX) $< ball.o -o $@ $(LDFLAGS)

%.o: %.cpp; $(CXX) $< -o $@ $(CXXFLAGS)

clean: ; rm -f bouncing.o bouncing ball.o bg.o bg MOG_function.o MOG_function


