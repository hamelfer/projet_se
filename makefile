CC = gcc

CFLAGS = -std=c2x -Wpedantic -Wall \
	-Wextra -Wconversion -Werror -fstack-protector-all -fpie \
	-pie -O2 -D_FORTIFY_SOURCE=2 -MMD	$(patsubst %, -I'%', $(VPATH)) \
	-D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=500 -Wfatal-errors \
	-lpthread -DDEBUG=0

TARGET = server
CLIENT = client

MODULES = matrix segment workera workerb utils

VPATH	= $(patsubst %, src/%, $(MODULES))

SRCS = $(patsubst %, %.c, $(MODULES))
SRCS += main.c
OBJS = $(patsubst %.c, obj/%.o, $(SRCS))

DEPS = $(OBJS:.o=.d)

all: $(TARGET) $(CLIENT)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT): client.c
	$(CC) $(CFLAGS) -o $@ $<

obj/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o : main.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(TARGET) $(CLIENT) $(OBJS) $(DEPS)

distclean: clean
	rm -f -v server_request_pipe /dev/shm/*

.PHONY: all clean distclean
