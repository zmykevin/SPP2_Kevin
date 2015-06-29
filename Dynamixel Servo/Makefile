TARGET		= servo_test
OBJS		= servo_test.o
SRCS		= $(OBJS:.o=.c)
INCLUDEDIRS	+= -I../dxl_sdk/include
LIBDIRS		+= -L../dxl_sdk/lib
CFLAGS		= $(INCLUDEDIRS) -W -Wall -O2
LDFLAGS		= $(LIBDIRS) -lm -ldxl

CC			= gcc

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	
.c.o:
	$(CC) -c $< $(CFLAGS)
	
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "file deleted."

dep:
	gccmakedep $(SRCS)
